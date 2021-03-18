#include "compiler.hh"

#include <libgen.h>
#include <unistd.h>

#include <cstring>
#include <climits>
#include <fstream>
#include <iostream>
#include <regex>

static std::string compiler_full_path()
{
    return "/usr/local/bin/vasmz80_oldstyle";   // TODO
}

static std::string execute_compiler(std::string const& path, std::string const& filename)
{
    char buffer[1024];

    std::string commandline = "cd " + path + " && " + compiler_full_path() + " -chklabels -L listing.txt -Fbin -autoexp -o rom.bin " + filename + " 2>&1";
    
    FILE* pipe = popen(commandline.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("Could not open executable " + compiler_full_path() + "!");
    
    std::string result;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr)
        result += buffer;
    if (pclose(pipe) != 0)
        throw std::runtime_error(result);
    return result;
}

static std::pair<std::string, std::string> find_project_path(std::string const& fullname)
{
    char* btmp = strdup(fullname.c_str());
    std::string bname = basename(btmp);
    free(btmp);
    
    btmp = strdup(fullname.c_str());
    std::string pname = dirname(btmp);
    free(btmp);
    
    return { pname, bname };
}

static void load_binary(std::string const& path, std::string const& file, SourceFile const& source, CompilerResult& result)
{
    std::ifstream instream(path + "/rom.bin", std::ios::in | std::ios::binary);
    if (instream.fail())
        throw std::runtime_error("File rom.bin could not be open.");
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
    
    Binary& binary = result.binaries[file] = {};
    if (binary.data.size() < data.size())
        binary.data.resize(data.size());
    for (size_t i = 0; i < data.size(); ++i)
        binary.data[i] = data[i];
    binary.address = source.expected_address;
}

static std::unordered_map<size_t, std::string> find_filenames(std::string const& path, CompilerResult& result)
{
    std::ifstream f(path + "/listing.txt");
    if (f.fail())
        throw std::runtime_error("File listing.txt does not exist or could not be opened.\n");
    
    enum Section { Sources, Other };
    Section section = Other;
    
    std::string line;
    std::unordered_map<size_t, std::string> ret;
    while (std::getline(f, line)) {
        if (section == Other) {
            if (line == "Sources:")
                section = Sources;
        } else {
            if (line[0] == 'F') {
                std::string file_number_s = line.substr(1, 2);
                size_t file_number = strtoul(file_number_s.c_str(), nullptr, 10);
                ret[file_number] = line.substr(5);
                result.debug.files[line.substr(5)] = 0;
            } else {
                section = Other;
            }
        }
    }
    
    return ret;
}

static void read_regular_line(std::unordered_map<size_t, std::string> const& filenames, std::string const& line,
                              CompilerResult& result, size_t& file_number, size_t& file_line)
{
    // read line
    std::string file_number_s = line.substr(1, 2);
    std::string file_line_s = line.substr(4, 4);
    std::string sourceline = line.substr(15);
    file_number = strtoul(file_number_s.c_str(), nullptr, 10);
    file_line = strtoul(file_line_s.c_str(), nullptr, 10);
    if (file_number == ULONG_MAX || file_line == ULONG_MAX)
        throw std::runtime_error("Invalid listing file format.");
    std::string filename;
    try {
        filename = filenames.at(file_number);
    } catch (std::out_of_range&) {
        throw std::runtime_error("Error in listing.txt: a sourceline line refers to a file number " + std::to_string(file_number) + ", which doesn't exist.");
    }
    
    // store line in `debug.source[filename]`
    result.debug.files.at(filename) = std::max(result.debug.files.at(filename), file_line);
    if (result.debug.source.find(filename) == result.debug.source.end())
        result.debug.source.insert({ filename, SourceAddresses {} });
    result.debug.source.at(filename).insert({ file_line, { sourceline } });
}

static size_t read_address(SourceFile const& source, std::unordered_map<size_t, std::string> const& filenames,
                           std::string const& line, CompilerResult& result, size_t file_number, size_t file_line)
{
    // read line
    std::string addr_s = line.substr(23, 4);
    unsigned long addr = strtoul(addr_s.c_str(), nullptr, 16);
    if (addr == ULONG_MAX)
        throw std::runtime_error("Invalid listing file format.");
    addr += source.expected_address;
    
    // store in source and location
    std::string const& filename = filenames.at(file_number);
    result.debug.source.at(filename)[file_line].address = addr;
    result.debug.location.insert({ addr, { filename, file_line } });
    
    // add bytes
    size_t pos = 30;
    std::vector<uint8_t>& bytes = result.debug.source.at(filename)[file_line].bytes;
    while (line.size() >= pos + 2) {
        if (!isxdigit(line[pos]))
            break;
        unsigned long byte = strtoul(line.substr(pos, 2).c_str(), nullptr, 16);
        if (byte == ULONG_MAX)
            break;
        bytes.push_back(byte);
        pos += 3;
    }
    return file_line;
}

static void load_listing(std::string const& path, SourceFile const& source, std::unordered_map<size_t, std::string> const& filenames,
                         CompilerResult& result)
{
    std::ifstream f(path + "/listing.txt");
    if (f.fail())
        throw std::runtime_error("File listing.txt does not exist or could not be opened.\n");
    
    enum Section { Source, Other, Symbols };
    
    std::string line;
    size_t file_number, file_line;
    Section section = Source;
    while (std::getline(f, line)) {
        if (line.empty())
            continue;
        if (line == "Sections:" || line == "Sources" )
            section = Other;
        else if (line == "Symbols:")
            section = Symbols;
        else if (section == Source && line[0] == 'F')    // regular sourceline line
            read_regular_line(filenames, line, result, file_number, file_line);
        else if (section == Source && line[0] == ' ')  // address
            file_line = read_address(source, filenames, line, result, file_number, file_line);
        else if (section == Symbols) {
            static std::regex regex1(R"(^([^\s][A-z0-9_]+)\s*EXPR\s*\([0-9]+=((0x)?[0-9A-Fa-f]+)\).*ABS.*)");
            static std::regex regex2(R"(^([^\s][A-z0-9_]+)\s+LAB\s*\(((0x)?[0-9A-Fa-f]+)\).*)");
            std::smatch m;
            if (std::regex_match(line, m, regex1) || std::regex_match(line, m, regex2)) {
                std::string symbol_name = m[1];
                unsigned long addr = strtoul(std::string(m[2]).c_str(), nullptr, 16);
                result.debug.symbols[symbol_name] = addr;
            }
            
        }
            /*
            } else if (section == SYMBOLS) {
                regmatch_t m[5] = { 0 };
                int r = regexec(&regex1, line, sizeof m, m, 0);
                if (r == REG_NOMATCH)
                    r = regexec(&regex2, line, sizeof m, m, 0);
                if (r == 0 && m[1].rm_so != -1 && m[2].rm_so != -1) {  // match
                    char symbol_name[512] = { 0 };
                    strncat(symbol_name, &line[m[1].rm_so], m[1].rm_eo - m[1].rm_so);
                    char addr_s[16] = { 0 };
                    strncat(addr_s, &line[m[2].rm_so], m[2].rm_eo - m[2].rm_so);
                    unsigned long addr = strtoul(addr_s, NULL, 16);
                    int n_symbols = ++di->n_symbols;
                    di->symbols = realloc(di->symbols, n_symbols * sizeof(DebugSymbol));
                    di->symbols[n_symbols - 1].symbol = strdup(symbol_name);
                    di->symbols[n_symbols - 1].addr = addr;
                }
             */
    }
}

void cleanup(std::string const& path)
{
    unlink((path + "/listing.txt").c_str());
    unlink((path + "/rom.bin").c_str());
}

CompilerResult compile(std::vector<SourceFile> const& sources)
{
    CompilerResult compiler_result;
    for (SourceFile const& source: sources) {
        auto [path, file] = find_project_path(source.filename);
        try {
            compiler_result.message += execute_compiler(path, source.filename);
        } catch (std::runtime_error& e) {
            compiler_result.error = e.what();
            break;
        }
        load_binary(path, file, source, compiler_result);
        auto filenames = find_filenames(path, compiler_result);
        load_listing(path, source, filenames, compiler_result);
        cleanup(path);
    }
    return compiler_result;
}
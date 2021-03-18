#include "compiler.hh"

#include <libgen.h>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <climits>

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

static void load_binary(std::string const& path, SourceFile const& file, CompilerResult& result)
{
    std::ifstream instream(path + "/rom.bin", std::ios::in | std::ios::binary);
    if (instream.fail())
        throw std::runtime_error("File rom.bin could not be open.");
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
    if (result.binary.size() < data.size() + file.expected_address)
        result.binary.resize(data.size() + file.expected_address);
    for (size_t i = 0; i < data.size(); ++i)
        result.binary[i + file.expected_address] = data[i];
}

static std::unordered_map<size_t, std::string> find_filenames(std::string const& path)
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
            } else {
                section = Other;
            }
        }
    }
    
    return ret;
}

static void load_listing(std::string const& path, SourceFile const& source, std::unordered_map<size_t, std::string> filenames,
                         CompilerResult& result)
{
    std::ifstream f(path + "/listing.txt");
    if (f.fail())
        throw std::runtime_error("File listing.txt does not exist or could not be opened.\n");
    
    enum Section { Source, Other };
    
    std::string line;
    size_t file_number, file_line;
    Section section = Source;
    while (std::getline(f, line)) {
        if (line.empty())
            continue;
        if (line == "Sections:" || line == "Symbols:" || line == "Sources" ) {
            section = Other;
        } else if (section == Source && line[0] == 'F') {   // regular sourceline line
            
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
            auto [it, _] = result.debug.source.insert_or_assign(filename, std::vector<SourceAddress>());
            SourceAddresses& addresses = it->second;
            if (addresses.size() < file_line)
                addresses.resize(file_line + 1, { "" });
            addresses[file_line] = { sourceline };
    
        } else if (section == Source && line[0] == ' ') {  // address
            // read line
            std::string addr_s = line.substr(23, 4);
            unsigned long addr = strtoul(addr_s.c_str(), nullptr, 16);
            if (addr == ULONG_MAX)
                throw std::runtime_error("Invalid listing file format.");
            addr += source.expected_address;
    
            // store in source and location
            std::string const& filename = filenames.at(file_number);
            result.debug.source.at(filename)[file_line].address = addr;
            result.debug.location[addr] = { filename, file_line };
            
            // add bytes
            size_t pos = 30;
            std::vector<uint8_t>& bytes = result.debug.source.at(filename)[file_line].bytes;
            while (line.size() >= pos + 2) {
                unsigned long byte = strtoul(line.substr(pos, 2).c_str(), nullptr, 16);
                if (byte == ULONG_MAX)
                    break;
                bytes.push_back(byte);
                pos += 3;
            }
        }
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
        load_binary(path, source, compiler_result);
        auto filenames = find_filenames(path);
        load_listing(path, source, filenames, compiler_result);
        cleanup(path);
    }
    return compiler_result;
}
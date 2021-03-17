#include "compiler.hh"

#include <libgen.h>
#include <cstring>
#include <fstream>

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

void load_binary(std::string const& path, SourceFile const& file, CompilerResult& result)
{
    std::ifstream instream(path + "/rom.bin", std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
    if (result.binary.size() < data.size() + file.expected_address)
        result.binary.resize(data.size() + file.expected_address);
    for (size_t i = 0; i < data.size(); ++i)
        result.binary[i + file.expected_address] = data[i];
}

void load_listing(std::string const& path, SourceFile const& file, CompilerResult& result)
{

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
        load_listing(path, source, compiler_result);
    }
    return compiler_result;
}
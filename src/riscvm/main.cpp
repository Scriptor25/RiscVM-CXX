#include <fstream>
#include <iostream>
#include <vector>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/VM.hpp>

static int exec(const char* pgm, const size_t size)
{
    RiscVM::Dump(pgm, size);

    RiscVM::VM vm;
    vm.Load(pgm, size);
    vm.Reset();

    do vm.Cycle();
    while (vm.Ok());

    return vm.Status();
}

static std::vector<char> read_bin(const std::string& filename)
{
    if (filename.empty())
    {
        std::vector<char> pgm;
        for (int c; (c = std::cin.get()) >= 0;)
            pgm.push_back(static_cast<char>(c));
        return pgm;
    }

    std::ifstream stream(filename, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    if (!stream.is_open())
        return {};

    std::vector<char> pgm(stream.tellg());
    stream.seekg(0);
    stream.read(pgm.data(), static_cast<std::streamsize>(pgm.size()));
    stream.close();

    return std::move(pgm);
}

static void write_bin(const std::string& filename, const char* data, const size_t size)
{
    std::ofstream stream(filename, std::ios_base::out | std::ios_base::binary);
    stream.write(data, static_cast<std::streamsize>(size));
    stream.close();
}

int main(const int argc, const char* const* argv)
{
    bool show_help = false;
    bool show_version = false;
    std::string in_filename;
    std::string in_type = "asm";
    std::string out_filename;
    std::string out_type = "bin";

    for (unsigned i = 1; i < argc; ++i)
    {
        if (std::string arg = argv[i]; arg == "--help" || arg == "-h") show_help = true;
        else if (arg == "--version" || arg == "-v") show_version = true;
        else if (arg == "--in-type" || arg == "-it") in_type = argv[++i];
        else if (arg == "--output" || arg == "-o") out_filename = argv[++i];
        else if (arg == "--out-type" || arg == "-ot") out_type = argv[++i];
        else in_filename = arg;
    }

    if (show_help)
    {
        std::cout << "RiscVM" << std::endl;
        std::cout << argv[0] << " <options> <filename>" << std::endl;
        std::cout << "OPTIONS" << std::endl;
        std::cout << "--help,     -h            -> display this text" << std::endl;
        std::cout << "--version,  -v            -> display version info" << std::endl;
        std::cout << "--in-type,  -it <type>    -> specify output filetype" << std::endl;
        std::cout << "--output,   -o <filename> -> specify output filename" << std::endl;
        std::cout << "--out-type, -ot <type>    -> specify output filetype" << std::endl;
        std::cout << "TYPE" << std::endl;
        std::cout << "asm  -> source assembly file" << std::endl;
        std::cout << "bin  -> raw binary file" << std::endl;
        std::cout << "elf  -> elf binary file" << std::endl;
        std::cout << "coff -> coff binary file" << std::endl;
        return 0;
    }

    if (show_version)
        std::cout << "RiscVM (version 1.0.0)" << std::endl;

    std::vector<char> pgm;

    // TODO: different input type: asm, bin, elf, coff, ...
    if (in_type == "asm")
    {
        std::ifstream stream(in_filename);
        if (stream.is_open())
            pgm = RiscVM::Assembler(stream).Parse();
        else pgm = RiscVM::Assembler(std::cin).Parse();
        stream.close();

        if (!out_filename.empty())
        {
            // TODO: different output type: bin, elf, coff, ...
            if (out_type == "bin")
            {
                write_bin(out_filename, pgm.data(), pgm.size());
            }
        }
    }
    else if (in_type == "bin")
    {
        pgm = read_bin(in_filename);
    }

    const auto status = exec(pgm.data(), pgm.size());
    std::cout << "Exit Code " << status << std::endl;
}

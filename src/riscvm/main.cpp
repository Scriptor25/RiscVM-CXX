#include <cstdarg>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <RiscVM/ArgParser.hpp>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/VM.hpp>

static int exec(const char* pgm, const size_t size)
{
    RiscVM::DumpRaw(pgm, size);
    RiscVM::Dump(pgm, size);

    RiscVM::VM vm;
    vm.Load(pgm, size);
    vm.Reset();

    auto& ecall_map = vm.ECallMap();
    ecall_map[0] = [](RiscVM::VM& vm_)
    {
        fputc(vm_.R(RiscVM::a0), stdout);
        fflush(stdout);
    };
    ecall_map[1] = [](RiscVM::VM& vm_)
    {
        fputs(vm_.Memory() + vm_.R(RiscVM::a0), stdout);
        fflush(stdout);
    };
    ecall_map[2] = [](RiscVM::VM& vm_)
    {
#ifdef _WIN32
        vfprintf(stdout, vm_.Memory() + vm_.R(RiscVM::a0), vm_.Memory() + vm_.R(RiscVM::a1));
#else
        vfprintf(stdout, vm_.Memory() + vm_.R(RiscVM::a0), reinterpret_cast<va_list>(vm_.Memory() + vm_.R(RiscVM::a1)));
#endif
        fflush(stdout);
    };
    ecall_map[3] = [](RiscVM::VM& vm_)
    {
        vm_.R(RiscVM::a0) = fgetc(stdin);
    };
    ecall_map[4] = [](RiscVM::VM& vm_)
    {
        fgets(vm_.Memory() + vm_.R(RiscVM::a0), vm_.R(RiscVM::a1), stdin);
    };
    ecall_map[5] = [](RiscVM::VM& vm_)
    {
#ifdef _WIN32
        vfscanf(stdin, vm_.Memory() + vm_.R(RiscVM::a0), vm_.Memory() + vm_.R(RiscVM::a1));
#else
        vfscanf(stdin, vm_.Memory() + vm_.R(RiscVM::a0), dynamic_cast<va_list>(vm_.Memory() + vm_.R(RiscVM::a1)));
#endif
    };
    ecall_map[120] = [](RiscVM::VM& vm_)
    {
        static std::random_device dev;
        static std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(vm_.R(RiscVM::a0), vm_.R(RiscVM::a1));
        vm_.R(RiscVM::a0) = static_cast<int32_t>(dist(rng));
    };
    ecall_map[127] = [](RiscVM::VM& vm_)
    {
        vm_.Ok() = false;
        vm_.Status() = vm_.R(RiscVM::a0);
    };

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
    RiscVM::ArgParser args({
        {"help", "print help and exit", {"-h", "--help"}},
        {"in-type", "specify input filetype (asm, bin, elf, coff)", {"--in-type", "-it"}, false},
        {"out-type", "specify output filetype (bin, elf, coff)", {"--out-type", "-ot"}, false},
        {"output", "specify output filename", {"--output", "-o"}, false},
        {"version", "print version", {"-v", "--version", "--info"}},
    });
    args.Parse(argc, argv);

    if (args.Flags["help"] || args.Flags["version"])
        std::cout << "RiscVM (version 1.0.0)" << std::endl;

    if (args.Flags["help"])
    {
        args.Print();
        return 0;
    }

    const auto& in_filename = args.Args.empty() ? "" : args.Args[0];
    const auto& out_filename = args.Get("output");
    const auto& in_type = args.Get("in-type", "asm");
    const auto& out_type = args.Get("out-type", "bin");

    std::vector<char> pgm;
    if (in_type == "asm")
    {
        RiscVM::LinkInfo link_info
        {
            {
                {".text", 2},
                {".data"},
                {".rodata"},
                {".bss"},
            }
        };

        if (in_filename.empty())
            RiscVM::Assembler::Assemble(std::cin, link_info, pgm);
        else
        {
            std::ifstream stream(in_filename);
            RiscVM::Assembler::Assemble(stream, link_info, pgm);
            stream.close();
        }

        if (!out_filename.empty())
        {
            if (out_type == "bin")
            {
                write_bin(out_filename, pgm.data(), pgm.size());
            }
            else if (out_type == "elf")
            {
                std::cerr << "output file format 'elf' is not YET supported" << std::endl;
            }
            else if (out_type == "coff")
            {
                std::cerr << "output file format 'coff' is not YET supported" << std::endl;
            }
            else
            {
                std::cerr << "output file format '" << out_type << "' is not supported" << std::endl;
            }
        }
    }
    else if (in_type == "bin")
    {
        pgm = read_bin(in_filename);
    }
    else if (in_type == "elf")
    {
        std::cerr << "input file format 'elf' is not YET supported" << std::endl;
        return 1;
    }
    else if (in_type == "coff")
    {
        std::cerr << "input file format 'coff' is not YET supported" << std::endl;
        return 1;
    }
    else
    {
        std::cerr << "input file format '" << in_type << "' is not supported" << std::endl;
        return 1;
    }

    const auto status = exec(pgm.data(), pgm.size());
    std::cout << "Exit Code " << status << std::endl;
}

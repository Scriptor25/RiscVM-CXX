#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/VM.hpp>

static void dump(const char* binary, const size_t size)
{
    constexpr unsigned width = 16;

    bool zero = false;
    for (unsigned i = 0; i < size; i += width)
    {
        if (zero)
        {
            for (unsigned j = 0; j < width; ++j)
                if (i + j < size && binary[i + j])
                    zero = false;

            if (zero && i + width < size) continue;
            printf("...\n");
        }

        printf("%04X | ", i);

        zero = true;
        for (unsigned j = 0; j < width; ++j)
        {
            if (i + j < size)
            {
                if (binary[i + j]) zero = false;
                printf("%02X ", static_cast<uint8_t>(binary[i + j]));
            }
            else printf("00 ");
        }

        printf("| ");

        for (unsigned j = 0; j < width; j++)
        {
            if (i + j < size && binary[i + j] >= 0x20)
                printf("%c", binary[i + j]);
            else printf(".");
        }

        printf("\n");
    }

    for (unsigned i = 0; i < size; i += 4)
    {
        const auto& data = *reinterpret_cast<const uint32_t*>(binary + i);
        if (!data) continue;

        printf("%08X: ", data);

        switch (data & 0b1111111)
        {
        case RiscVM::RV32_64G_OP:
            RiscVM::Format::R{.Data = data}.Dump();
            break;
        case RiscVM::RV32_64G_OP_IMM:
        case RiscVM::RV32_64G_JALR:
        case RiscVM::RV32_64G_LOAD:
        case RiscVM::RV32_64G_SYSTEM:
        case RiscVM::RV32_64G_MISC_MEM:
            RiscVM::Format::I{.Data = data}.Dump();
            break;
        case RiscVM::RV32_64G_STORE:
            RiscVM::Format::S{.Data = data}.Dump();
            break;
        case RiscVM::RV32_64G_BRANCH:
            RiscVM::Format::B{.Data = data}.Dump();
            break;
        case RiscVM::RV32_64G_LUI:
        case RiscVM::RV32_64G_AUIPC:
            RiscVM::Format::U{.Data = data}.Dump();
            break;
        case RiscVM::RV32_64G_JAL:
            RiscVM::Format::J{.Data = data}.Dump();
            break;
        default:
            printf("?");
            break;
        }

        printf("\n");
    }

    fflush(stdout);
}

static int exec(const char* pgm, const size_t size)
{
    dump(pgm, size);

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
        std::unique_ptr<RiscVM::Assembler> as;
        std::ifstream stream(in_filename);
        if (in_filename.empty()) as = std::make_unique<RiscVM::Assembler>(std::cin);
        else as = std::make_unique<RiscVM::Assembler>(stream);
        while (!as->At(RiscVM::TokenType_EOF))
            as->Parse();
        pgm = as->Link();
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

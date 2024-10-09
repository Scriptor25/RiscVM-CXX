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

static std::vector<char> from_binary(const std::string& filename)
{
    std::ifstream stream(filename, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    if (!stream) return {};

    std::vector<char> pgm(stream.tellg());
    stream.seekg(0);
    stream.read(pgm.data(), static_cast<std::streamsize>(pgm.size()));
    stream.close();

    return std::move(pgm);
}

static std::vector<char> from_source(const std::string& filename)
{
    std::ifstream stream(filename);
    if (!stream) return {};

    RiscVM::Assembler as(stream);
    while (!as.At(RiscVM::TokenType_EOF))
        as.Parse();
    auto pgm = as.Link();

    return std::move(pgm);
}

int main(const int argc, const char* const* argv)
{
    std::map<std::string, bool> flags;
    std::vector<std::string> filenames;

    for (unsigned i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg[0] == '-')
        {
            arg = arg.substr(1);
            flags[arg] = true;
            continue;
        }

        filenames.push_back(arg);
    }

    if (flags["h"])
    {
        std::cout << argv[0] << " <options> <filename>..." << std::endl;
        return 0;
    }

    if (filenames.empty())
        return 1;

    const auto type = (flags["b"] & 0b1) << 1 | flags["s"] & 0b1;

    std::vector<char> pgm;
    switch (type)
    {
    case 0b01:
        pgm = from_source(filenames[0]);
        break;
    case 0b10:
        pgm = from_binary(filenames[0]);
        break;
    default:
        return 1;
    }

    if (flags["o"])
    {
        std::ofstream stream(filenames[1], std::ios_base::out | std::ios_base::binary);
        stream.write(pgm.data(), static_cast<std::streamsize>(pgm.size()));
        stream.close();
    }

    const auto status = exec(pgm.data(), pgm.size());
    std::cout << "Exit Code " << status << std::endl;
}

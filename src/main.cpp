#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/Instruction.hpp>
#include <RiscVM/ISA.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>
#include <RiscVM/Symbol.hpp>
#include <RiscVM/VM.hpp>

static void dump(char* binary, const size_t size)
{
    constexpr size_t width = 16;

    for (size_t i = 0; i < size; i += width)
    {
        printf("%04X | ", static_cast<unsigned>(i));

        for (size_t j = 0; j < width; j++)
        {
            if (i + j < size)
                printf("%02X ", static_cast<uint8_t>(binary[i + j]));
            else printf("00 ");
        }

        printf("| ");

        for (size_t j = 0; j < width; j++)
        {
            if (i + j < size && binary[i + j] >= 0x20)
                printf("%c", binary[i + j]);
            else printf(".");
        }

        printf("\n");
    }

    for (size_t i = 0; i < size; i += 4)
    {
        const auto data = *reinterpret_cast<uint32_t*>(&binary[i]);
        printf("%08X: ", data);

        switch (data & 0b1111111)
        {
        case RiscVM::RV32_64G_OP:
            RiscVM::Format::R{.Data = data}.Dump();
            break;
        case RiscVM::RV32_64G_OP_IMM:
        case RiscVM::RV32_64G_JALR:
        case RiscVM::RV32_64G_LOAD:
        case RiscVM::RV32_64G_MISC_MEM:
        case RiscVM::RV32_64G_SYSTEM:
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
        fflush(stdout);
    }
}

static int read_binary(const int argc, const char** argv)
{
    std::ifstream stream(argv[1], std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    if (!stream)
        return 1;

    std::vector<char> binary(stream.tellg());
    stream.seekg(0);
    stream.read(binary.data(), static_cast<std::streamsize>(binary.size()));
    stream.close();

    dump(binary.data(), binary.size());

    RiscVM::VM vm;
    vm.R(RiscVM::sp) = 1024;
    vm.Load(binary.data(), binary.size());
    while (vm.Cycle());

    return 0;
}

int main(const int argc, const char* const* argv)
{
    std::ifstream stream(argv[1]);
    if (!stream)
        return 1;

    RiscVM::Assembler as(stream);
    while (!as.At(RiscVM::TokenType_EOF))
        as.Parse();

    RiscVM::VM vm;
    as.Link(vm);
    dump(vm.Memory(), 152);

    unsigned c = 0;
    while (vm.Cycle()) c++;

    std::cout << "Cycles: " << c << std::endl;
    std::cout << "Exit Code " << vm.Status() << std::endl;
}

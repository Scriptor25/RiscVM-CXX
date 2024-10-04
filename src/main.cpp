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

    fflush(stdout);
}

static int exec(const char* pgm, const size_t size)
{
    dump(pgm, size);

    RiscVM::VM vm;
    vm.Load(pgm, size);

    do vm.Cycle();
    while (vm.Ok());

    std::cout << "Exit Code " << vm.Status() << std::endl;
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
    const auto pgm = from_source(argv[1]);
    return exec(pgm.data(), pgm.size());
}

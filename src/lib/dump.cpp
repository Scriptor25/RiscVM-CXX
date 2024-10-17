#include <RiscVM/ISA.hpp>
#include <RiscVM/RiscVM.hpp>

void RiscVM::DumpRaw(const char* binary, const size_t size)
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

void RiscVM::Dump(const char* binary, const size_t size)
{
    for (unsigned i = 0; i < size; i += 4)
    {
        const auto& data = *reinterpret_cast<const uint32_t*>(binary + i);
        if (!data) continue;

        printf("%08X: ", data);

        switch (data & 0b1111111)
        {
        case RV32_64G_OP:
            Format::R{.Data = data}.Dump();
            break;
        case RV32_64G_OP_IMM:
        case RV32_64G_JALR:
        case RV32_64G_LOAD:
        case RV32_64G_SYSTEM:
        case RV32_64G_MISC_MEM:
            Format::I{.Data = data}.Dump();
            break;
        case RV32_64G_STORE:
            Format::S{.Data = data}.Dump();
            break;
        case RV32_64G_BRANCH:
            Format::B{.Data = data}.Dump();
            break;
        case RV32_64G_LUI:
        case RV32_64G_AUIPC:
            Format::U{.Data = data}.Dump();
            break;
        case RV32_64G_JAL:
            Format::J{.Data = data}.Dump();
            break;
        default:
            printf("?");
            break;
        }

        printf("\n");
    }

    fflush(stdout);
}

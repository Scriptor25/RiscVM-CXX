#pragma once

#include <cstdint>
#include <string>

namespace RiscVM
{
    enum RV32_64G
    {
        RV32_64G_LOAD = 0b0000011,
        RV32_64G_LOAD_FP = 0b0000111,
        RV32_64G_CUSTOM_0 = 0b0001011,
        RV32_64G_MISC_MEM = 0b0001111,
        RV32_64G_OP_IMM = 0b0010011,
        RV32_64G_AUIPC = 0b0010111,
        RV32_64G_OP_IMM_32 = 0b0011011,

        RV32_64G_STORE = 0b0100011,
        RV32_64G_STORE_FP = 0b0100111,
        RV32_64G_CUSTOM_1 = 0b0101011,
        RV32_64G_AMO = 0b0101111,
        RV32_64G_OP = 0b0110011,
        RV32_64G_LUI = 0b0110111,
        RV32_64G_OP_32 = 0b0111011,

        RV32_64G_MADD = 0b1000011,
        RV32_64G_MSUB = 0b1000111,
        RV32_64G_NMSUB = 0b1001011,
        RV32_64G_NMADD = 0b1001111,
        RV32_64G_OP_FP = 0b1010011,
        RV32_64G_CUSTOM_2 = 0b1011011,

        RV32_64G_BRANCH = 0b1100011,
        RV32_64G_JALR = 0b1100111,
        RV32_64G_JAL = 0b1101111,
        RV32_64G_SYSTEM = 0b1110011,
        RV32_64G_CUSTOM_3 = 0b1111011,
    };

    enum RV32IM
    {
        RV32IM_LUI = RV32_64G_LUI,
        RV32IM_AUIPC = RV32_64G_AUIPC,
        RV32IM_JAL = RV32_64G_JAL,
        RV32IM_JALR = RV32_64G_JALR,
        RV32IM_BEQ = RV32_64G_BRANCH,
        RV32IM_BNE = 0b001 << 7 | RV32_64G_BRANCH,
        RV32IM_BLT = 0b100 << 7 | RV32_64G_BRANCH,
        RV32IM_BGE = 0b101 << 7 | RV32_64G_BRANCH,
        RV32IM_BLTU = 0b110 << 7 | RV32_64G_BRANCH,
        RV32IM_BGEU = 0b111 << 7 | RV32_64G_BRANCH,
        RV32IM_LB = RV32_64G_LOAD,
        RV32IM_LH = 0b001 << 7 | RV32_64G_LOAD,
        RV32IM_LW = 0b010 << 7 | RV32_64G_LOAD,
        RV32IM_LBU = 0b100 << 7 | RV32_64G_LOAD,
        RV32IM_LHU = 0b101 << 7 | RV32_64G_LOAD,
        RV32IM_SB = RV32_64G_STORE,
        RV32IM_SH = 0b001 << 7 | RV32_64G_STORE,
        RV32IM_SW = 0b010 << 7 | RV32_64G_STORE,
        RV32IM_ADDI = RV32_64G_OP_IMM,
        RV32IM_SLTI = 0b010 << 7 | RV32_64G_OP_IMM,
        RV32IM_SLTIU = 0b011 << 7 | RV32_64G_OP_IMM,
        RV32IM_XORI = 0b100 << 7 | RV32_64G_OP_IMM,
        RV32IM_ORI = 0b110 << 7 | RV32_64G_OP_IMM,
        RV32IM_ANDI = 0b111 << 7 | RV32_64G_OP_IMM,
        RV32IM_SLLI = 0b001 << 7 | RV32_64G_OP_IMM,
        RV32IM_SRLI = 0b101 << 7 | RV32_64G_OP_IMM,
        RV32IM_SRAI = 0b0100000 << 10 | 0b101 << 7 | RV32_64G_OP_IMM,
        RV32IM_ADD = RV32_64G_OP,
        RV32IM_SUB = 0b0100000 << 10 | RV32_64G_OP,
        RV32IM_SLL = 0b001 << 7 | RV32_64G_OP,
        RV32IM_SLT = 0b010 << 7 | RV32_64G_OP,
        RV32IM_SLTU = 0b011 << 7 | RV32_64G_OP,
        RV32IM_XOR = 0b100 << 7 | RV32_64G_OP,
        RV32IM_SRL = 0b101 << 7 | RV32_64G_OP,
        RV32IM_SRA = 0b0100000 << 10 | 0b101 << 7 | RV32_64G_OP,
        RV32IM_OR = 0b110 << 7 | RV32_64G_OP,
        RV32IM_AND = 0b111 << 7 | RV32_64G_OP,
        RV32IM_FENCE = RV32_64G_MISC_MEM,
        RV32IM_ECALL = RV32_64G_SYSTEM,
        RV32IM_EBREAK = 0b1 << 20 | RV32_64G_SYSTEM,

        RV32IM_MUL = 0b0000001 << 10 | 0b000 << 7 | RV32_64G_OP,
        RV32IM_MULH = 0b0000001 << 10 | 0b001 << 7 | RV32_64G_OP,
        RV32IM_MULHSU = 0b0000001 << 10 | 0b010 << 7 | RV32_64G_OP,
        RV32IM_MULHU = 0b0000001 << 10 | 0b011 << 7 | RV32_64G_OP,
        RV32IM_DIV = 0b0000001 << 10 | 0b100 << 7 | RV32_64G_OP,
        RV32IM_DIVU = 0b0000001 << 10 | 0b101 << 7 | RV32_64G_OP,
        RV32IM_REM = 0b0000001 << 10 | 0b110 << 7 | RV32_64G_OP,
        RV32IM_REMU = 0b0000001 << 10 | 0b111 << 7 | RV32_64G_OP,
    };

    enum Register
    {
        zero = 0, ra = 1, sp = 2, gp = 3, tp = 4, t0 = 5, t1 = 6, t2 = 7, s0 = 8, s1 = 9, a0 = 10, a1 = 11, a2 = 12,
        a3 = 13, a4 = 14, a5 = 15, a6 = 16, a7 = 17, s2 = 18, s3 = 19, s4 = 20, s5 = 21, s6 = 22, s7 = 23, s8 = 24,
        s9 = 25, s10 = 26, s11 = 27, t3 = 28, t4 = 29, t5 = 30, t6 = 31
    };

    namespace Format
    {
        union R
        {
            void Dump() const;

            struct
            {
                uint32_t Opcode : 7;
                uint32_t Rd : 5;
                uint32_t Func3 : 3;
                uint32_t Rs1 : 5;
                uint32_t Rs2 : 5;
                uint32_t Func7 : 7;
            };

            uint32_t Data;
        };

        union I
        {
            [[nodiscard]] int32_t Immediate() const;
            [[nodiscard]] uint32_t UImmediate() const;

            void Immediate(int32_t);

            void Dump() const;

            struct
            {
                uint32_t Opcode : 7;
                uint32_t Rd : 5;
                uint32_t Func3 : 3;
                uint32_t Rs1 : 5;
                uint32_t Imm_10_0 : 11;
                uint32_t Imm_11 : 1;
            };

            uint32_t Data;
        };

        union S
        {
            [[nodiscard]] int32_t Immediate() const;
            [[nodiscard]] uint32_t UImmediate() const;

            void Immediate(int32_t);

            void Dump() const;

            struct
            {
                uint32_t Opcode : 7;
                uint32_t Imm_4_0 : 5;
                uint32_t Func3 : 3;
                uint32_t Rs1 : 5;
                uint32_t Rs2 : 5;
                uint32_t Imm_10_5 : 6;
                uint32_t Imm_11 : 1;
            };

            uint32_t Data;
        };

        union B
        {
            [[nodiscard]] int32_t Immediate() const;
            [[nodiscard]] uint32_t UImmediate() const;

            void Immediate(int32_t);

            void Dump() const;

            struct
            {
                uint32_t Opcode : 7;
                uint32_t Imm_11 : 1;
                uint32_t Imm_4_1 : 4;
                uint32_t Func3 : 3;
                uint32_t Rs1 : 5;
                uint32_t Rs2 : 5;
                uint32_t Imm_10_5 : 6;
                uint32_t Imm_12 : 1;
            };

            uint32_t Data;
        };

        union U
        {
            [[nodiscard]] int32_t Immediate() const;
            [[nodiscard]] uint32_t UImmediate() const;

            void Immediate(int32_t);

            void Dump() const;

            struct
            {
                uint32_t Opcode : 7;
                uint32_t Rd : 5;
                uint32_t Imm_30_12 : 19;
                uint32_t Imm_31 : 1;
            };

            uint32_t Data;
        };

        union J
        {
            [[nodiscard]] int32_t Immediate() const;
            [[nodiscard]] uint32_t UImmediate() const;

            void Immediate(int32_t);

            void Dump() const;

            struct
            {
                uint32_t Opcode : 7;
                uint32_t Rd : 5;
                uint32_t Imm_19_12 : 8;
                uint32_t Imm_11 : 1;
                uint32_t Imm_10_1 : 10;
                uint32_t Imm_20 : 1;
            };

            uint32_t Data;
        };
    }

    uint32_t ImmBits(uint32_t data, uint32_t end, uint32_t beg);
    uint32_t Extend(uint32_t bit, uint32_t n);

    uint32_t Rd(uint32_t);
    uint32_t Rs1(uint32_t);
    uint32_t Rs2(uint32_t);

    int32_t ImmediateI(uint32_t);
    int32_t ImmediateS(uint32_t);
    int32_t ImmediateB(uint32_t);
    int32_t ImmediateU(uint32_t);
    int32_t ImmediateJ(uint32_t);

    uint32_t UImmediateI(uint32_t);

    const char* RegisterName(uint32_t);
    const char* RegisterName(Register);
    bool IsRegister(const std::string&);
    Register GetRegister(const std::string&);

    const char* InstructionName(uint32_t);
    const char* InstructionName(RV32IM);
    bool IsInstruction(const std::string&);
    RV32IM GetInstruction(const std::string&);
    RV32IM GetInstruction(uint32_t);
}

#include <cstdio>
#include <unordered_map>
#include <RiscVM/ISA.hpp>

void RiscVM::Format::R::Dump() const
{
    printf(
        "%-7s %s,%s,%s",
        InstructionName(Data),
        RegisterName(Rd),
        RegisterName(Rs1),
        RegisterName(Rs2));
}

int32_t RiscVM::Format::I::Immediate() const
{
    return static_cast<int32_t>(Extend(Imm_11, 21) << 11 | Imm_10_0);
}

uint32_t RiscVM::Format::I::UImmediate() const
{
    return Imm_11 << 11 | Imm_10_0;
}

void RiscVM::Format::I::Immediate(const int32_t imm)
{
    Imm_10_0 = imm & 0b11111111111;
    Imm_11 = imm < 0 ? 0b1 : 0b0;
}

void RiscVM::Format::I::Dump() const
{
    if (Opcode == RV32_64G_LOAD || Opcode == RV32_64G_JALR)
        printf(
            "%-7s %s,%d(%s)",
            InstructionName(Data),
            RegisterName(Rd),
            Immediate(),
            RegisterName(Rs1));
    else
        printf(
            "%-7s %s,%s,%d",
            InstructionName(Data),
            RegisterName(Rd),
            RegisterName(Rs1),
            Immediate());
}

int32_t RiscVM::Format::S::Immediate() const
{
    return static_cast<int32_t>(Extend(Imm_11, 21) << 11 | Imm_10_5 << 5 | Imm_4_0);
}

uint32_t RiscVM::Format::S::UImmediate() const
{
    return Imm_11 << 11 | Imm_10_5 << 5 | Imm_4_0;
}

void RiscVM::Format::S::Immediate(const int32_t imm)
{
    Imm_4_0 = imm & 0b11111;
    Imm_10_5 = imm >> 5 & 0b111111;
    Imm_11 = imm < 0 ? 0b1 : 0b0;
}

void RiscVM::Format::S::Dump() const
{
    printf(
        "%-7s %s,%d(%s)",
        InstructionName(Data),
        RegisterName(Rs1),
        Immediate(),
        RegisterName(Rs2));
}

int32_t RiscVM::Format::B::Immediate() const
{
    return static_cast<int32_t>(Extend(Imm_12, 20) << 12 | Imm_11 << 11 | Imm_10_5 << 5 | Imm_4_1 << 1);
}

uint32_t RiscVM::Format::B::UImmediate() const
{
    return Imm_12 << 12 | Imm_11 << 11 | Imm_10_5 << 5 | Imm_4_1 << 1;
}

void RiscVM::Format::B::Immediate(const int32_t imm)
{
    Imm_4_1 = imm >> 1 & 0b1111;
    Imm_10_5 = imm >> 5 & 0b111111;
    Imm_11 = imm >> 11 & 0b1;
    Imm_12 = imm < 0 ? 0b1 : 0b0;
}

void RiscVM::Format::B::Dump() const
{
    printf(
        "%-7s %s,%s,%d",
        InstructionName(Data),
        RegisterName(Rs1),
        RegisterName(Rs2),
        Immediate());
}

int32_t RiscVM::Format::U::Immediate() const
{
    return static_cast<int32_t>(Imm_31 << 31 | Imm_30_12 << 12);
}

uint32_t RiscVM::Format::U::UImmediate() const
{
    return Imm_31 << 31 | Imm_30_12 << 12;
}

void RiscVM::Format::U::Immediate(const int32_t imm)
{
    Imm_30_12 = imm >> 12 & 0b1111111111111111111;
    Imm_31 = imm < 0 ? 0b1 : 0b0;
}

void RiscVM::Format::U::Dump() const
{
    printf(
        "%-7s %s,%d",
        InstructionName(Data),
        RegisterName(Rd),
        Immediate());
}

int32_t RiscVM::Format::J::Immediate() const
{
    return static_cast<int32_t>(Extend(Imm_20, 12) << 20 | Imm_19_12 << 12 | Imm_11 << 11 | Imm_10_1 << 1);
}

uint32_t RiscVM::Format::J::UImmediate() const
{
    return Imm_20 << 20 | Imm_19_12 << 12 | Imm_11 << 11 | Imm_10_1 << 1;
}

void RiscVM::Format::J::Immediate(const int32_t imm)
{
    Imm_10_1 = imm >> 1 & 0b1111111111;
    Imm_11 = imm >> 11 & 0b1;
    Imm_19_12 = imm >> 12 & 0b11111111;
    Imm_20 = imm < 0 ? 0b1 : 0b0;
}

void RiscVM::Format::J::Dump() const
{
    printf(
        "%-7s %s,%d",
        InstructionName(Data),
        RegisterName(Rd),
        Immediate());
}

uint32_t RiscVM::ImmBits(const uint32_t data, const uint32_t end, const uint32_t beg)
{
    uint32_t mask = 0;
    for (unsigned i = beg; i <= end; ++i)
        mask = mask << 1 | 0b1;
    return data >> beg & mask;
}

uint32_t RiscVM::Extend(const uint32_t bit, const uint32_t n)
{
    uint32_t mask = 0;
    for (uint32_t i = 0; i < n; ++i)
        mask = mask << 1 | bit;
    return mask;
}

uint32_t RiscVM::Rd(const uint32_t data)
{
    return data >> 7 & 0b11111;
}

uint32_t RiscVM::Rs1(const uint32_t data)
{
    return data >> 15 & 0b11111;
}

uint32_t RiscVM::Rs2(const uint32_t data)
{
    return data >> 20 & 0b11111;
}

int32_t RiscVM::ImmediateI(const uint32_t data)
{
    const auto imm_11 = ImmBits(data, 31, 31);
    const auto imm_10_0 = ImmBits(data, 30, 20);
    return static_cast<int32_t>(Extend(imm_11, 21) << 11 | imm_10_0);
}

int32_t RiscVM::ImmediateS(const uint32_t data)
{
    const auto imm_11 = ImmBits(data, 31, 31);
    const auto imm_10_5 = ImmBits(data, 30, 25);
    const auto imm_4_0 = ImmBits(data, 11, 7);
    return static_cast<int32_t>(Extend(imm_11, 21) << 11 | imm_10_5 << 5 | imm_4_0);
}

int32_t RiscVM::ImmediateB(const uint32_t data)
{
    const auto imm_12 = ImmBits(data, 31, 31);
    const auto imm_11 = ImmBits(data, 7, 7);
    const auto imm_10_5 = ImmBits(data, 30, 25);
    const auto imm_4_1 = ImmBits(data, 11, 8);
    return static_cast<int32_t>(Extend(imm_12, 20) << 12 | imm_11 << 11 | imm_10_5 << 5 | imm_4_1 << 1);
}

int32_t RiscVM::ImmediateU(const uint32_t data)
{
    return static_cast<int32_t>(ImmBits(data, 31, 12) << 12);
}

int32_t RiscVM::ImmediateJ(const uint32_t data)
{
    const auto imm_20 = ImmBits(data, 31, 31);
    const auto imm_19_12 = ImmBits(data, 19, 12);
    const auto imm_11 = ImmBits(data, 20, 20);
    const auto imm_10_1 = ImmBits(data, 30, 21);
    return static_cast<int32_t>(Extend(imm_20, 12) << 20 | imm_19_12 << 12 | imm_11 << 11 | imm_10_1 << 1);
}

uint32_t RiscVM::UImmediateI(const uint32_t data)
{
    return ImmBits(data, 31, 20);
}

static std::unordered_map<std::string, RiscVM::Register> string_to_register
{
    {"zero", RiscVM::zero}, {"ra", RiscVM::ra}, {"sp", RiscVM::sp}, {"gp", RiscVM::gp}, {"tp", RiscVM::tp},
    {"t0", RiscVM::t0}, {"t1", RiscVM::t1}, {"t2", RiscVM::t2}, {"s0", RiscVM::s0}, {"s1", RiscVM::s1},
    {"a0", RiscVM::a0}, {"a1", RiscVM::a1}, {"a2", RiscVM::a2}, {"a3", RiscVM::a3}, {"a4", RiscVM::a4},
    {"a5", RiscVM::a5}, {"a6", RiscVM::a6}, {"a7", RiscVM::a7}, {"s2", RiscVM::s2}, {"s3", RiscVM::s3},
    {"s4", RiscVM::s4}, {"s5", RiscVM::s5}, {"s6", RiscVM::s6}, {"s7", RiscVM::s7}, {"s8", RiscVM::s8},
    {"s9", RiscVM::s9}, {"s10", RiscVM::s10}, {"s11", RiscVM::s11}, {"t3", RiscVM::t3}, {"t4", RiscVM::t4},
    {"t5", RiscVM::t5}, {"t6", RiscVM::t6},

    {"x0", RiscVM::zero}, {"x1", RiscVM::ra}, {"x2", RiscVM::sp}, {"x3", RiscVM::gp}, {"x4", RiscVM::tp},
    {"x5", RiscVM::t0}, {"x6", RiscVM::t1}, {"x7", RiscVM::t2}, {"x8", RiscVM::s0}, {"x9", RiscVM::s1},
    {"x10", RiscVM::a0}, {"x11", RiscVM::a1}, {"x12", RiscVM::a2}, {"x13", RiscVM::a3}, {"x14", RiscVM::a4},
    {"x15", RiscVM::a5}, {"x16", RiscVM::a6}, {"x17", RiscVM::a7}, {"x18", RiscVM::s2}, {"x19", RiscVM::s3},
    {"x20", RiscVM::s4}, {"x21", RiscVM::s5}, {"x22", RiscVM::s6}, {"x23", RiscVM::s7}, {"x24", RiscVM::s8},
    {"x25", RiscVM::s9}, {"x26", RiscVM::s10}, {"x27", RiscVM::s11}, {"x28", RiscVM::t3}, {"x29", RiscVM::t4},
    {"x30", RiscVM::t5}, {"x31", RiscVM::t6},
};

static std::unordered_map<RiscVM::Register, const char*> register_to_string
{
    {RiscVM::zero, "zero"}, {RiscVM::ra, "ra"}, {RiscVM::sp, "sp"}, {RiscVM::gp, "gp"}, {RiscVM::tp, "tp"},
    {RiscVM::t0, "t0"}, {RiscVM::t1, "t1"}, {RiscVM::t2, "t2"}, {RiscVM::s0, "s0"}, {RiscVM::s1, "s1"},
    {RiscVM::s2, "s2"}, {RiscVM::a0, "a0"}, {RiscVM::a1, "a1"}, {RiscVM::a2, "a2"}, {RiscVM::a3, "a3"},
    {RiscVM::a4, "a4"}, {RiscVM::a5, "a5"}, {RiscVM::a6, "a6"}, {RiscVM::a7, "a7"}, {RiscVM::s2, "s2"},
    {RiscVM::s3, "s3"}, {RiscVM::s4, "s4"}, {RiscVM::s5, "s5"}, {RiscVM::s6, "s6"}, {RiscVM::s7, "s7"},
    {RiscVM::s8, "s8"}, {RiscVM::s9, "s9"}, {RiscVM::s10, "s10"}, {RiscVM::s11, "s11"}, {RiscVM::t3, "t3"},
    {RiscVM::t4, "t4"}, {RiscVM::t5, "t5"}, {RiscVM::t6, "t6"},
};

static std::unordered_map<std::string, uint32_t> string_to_isa
{
    {"lui", RiscVM::RV32I_LUI}, {"auipc", RiscVM::RV32I_AUIPC}, {"jal", RiscVM::RV32I_JAL},
    {"jalr", RiscVM::RV32I_JALR}, {"beq", RiscVM::RV32I_BEQ}, {"bne", RiscVM::RV32I_BNE},
    {"blt", RiscVM::RV32I_BLT}, {"bge", RiscVM::RV32I_BGE}, {"bltu", RiscVM::RV32I_BLTU},
    {"bgeu", RiscVM::RV32I_BGEU}, {"lb", RiscVM::RV32I_LB}, {"lh", RiscVM::RV32I_LH}, {"lw", RiscVM::RV32I_LW},
    {"lbu", RiscVM::RV32I_LBU}, {"lhu", RiscVM::RV32I_LHU}, {"sb", RiscVM::RV32I_SB}, {"sh", RiscVM::RV32I_SH},
    {"sw", RiscVM::RV32I_SW}, {"addi", RiscVM::RV32I_ADDI}, {"slti", RiscVM::RV32I_SLTI},
    {"sltiu", RiscVM::RV32I_SLTIU}, {"xori", RiscVM::RV32I_XORI}, {"ori", RiscVM::RV32I_ORI},
    {"andi", RiscVM::RV32I_ANDI}, {"slli", RiscVM::RV32I_SLLI}, {"srli", RiscVM::RV32I_SRLI},
    {"srai", RiscVM::RV32I_SRAI}, {"add", RiscVM::RV32I_ADD}, {"sub", RiscVM::RV32I_SUB},
    {"sll", RiscVM::RV32I_SLL}, {"slt", RiscVM::RV32I_SLT}, {"sltu", RiscVM::RV32I_SLTU},
    {"xor", RiscVM::RV32I_XOR}, {"srl", RiscVM::RV32I_SRL}, {"sra", RiscVM::RV32I_SRA}, {"or", RiscVM::RV32I_OR},
    {"and", RiscVM::RV32I_AND}, {"fence", RiscVM::RV32I_FENCE}, {"ecall", RiscVM::RV32I_ECALL},
    {"ebreak", RiscVM::RV32I_EBREAK}, {"mul", RiscVM::RV32M_MUL}, {"mulh", RiscVM::RV32M_MULH},
    {"mulhsu", RiscVM::RV32M_MULHSU}, {"mulhu", RiscVM::RV32M_MULHU}, {"div", RiscVM::RV32M_DIV},
    {"divu", RiscVM::RV32M_DIVU}, {"rem", RiscVM::RV32M_REM}, {"remu", RiscVM::RV32M_REMU},
};

static std::unordered_map<uint32_t, const char*> isa_to_string
{
    {RiscVM::RV32I_LUI, "lui"}, {RiscVM::RV32I_AUIPC, "auipc"}, {RiscVM::RV32I_JAL, "jal"},
    {RiscVM::RV32I_JALR, "jalr"}, {RiscVM::RV32I_BEQ, "beq"}, {RiscVM::RV32I_BNE, "bne"},
    {RiscVM::RV32I_BLT, "blt"}, {RiscVM::RV32I_BGE, "bge"}, {RiscVM::RV32I_BLTU, "bltu"},
    {RiscVM::RV32I_BGEU, "bgeu"}, {RiscVM::RV32I_LB, "lb"}, {RiscVM::RV32I_LH, "lh"}, {RiscVM::RV32I_LW, "lw"},
    {RiscVM::RV32I_LBU, "lbu"}, {RiscVM::RV32I_LHU, "lhu"}, {RiscVM::RV32I_SB, "sb"}, {RiscVM::RV32I_SH, "sh"},
    {RiscVM::RV32I_SW, "sw"}, {RiscVM::RV32I_ADDI, "addi"}, {RiscVM::RV32I_SLTI, "slti"},
    {RiscVM::RV32I_SLTIU, "sltiu"}, {RiscVM::RV32I_XORI, "xori"}, {RiscVM::RV32I_ORI, "ori"},
    {RiscVM::RV32I_ANDI, "andi"}, {RiscVM::RV32I_SLLI, "slli"}, {RiscVM::RV32I_SRLI, "srli"},
    {RiscVM::RV32I_SRAI, "srai"}, {RiscVM::RV32I_ADD, "add"}, {RiscVM::RV32I_SUB, "sub"},
    {RiscVM::RV32I_SLL, "sll"}, {RiscVM::RV32I_SLT, "slt"}, {RiscVM::RV32I_SLTU, "sltu"},
    {RiscVM::RV32I_XOR, "xor"}, {RiscVM::RV32I_SRL, "srl"}, {RiscVM::RV32I_SRA, "sra"}, {RiscVM::RV32I_OR, "or"},
    {RiscVM::RV32I_AND, "and"}, {RiscVM::RV32I_FENCE, "fence"}, {RiscVM::RV32I_ECALL, "ecall"},
    {RiscVM::RV32I_EBREAK, "ebreak"}, {RiscVM::RV32M_MUL, "mul"}, {RiscVM::RV32M_MULH, "mulh"},
    {RiscVM::RV32M_MULHSU, "mulhsu"}, {RiscVM::RV32M_MULHU, "mulhu"}, {RiscVM::RV32M_DIV, "div"},
    {RiscVM::RV32M_DIVU, "divu"}, {RiscVM::RV32M_REM, "rem"}, {RiscVM::RV32M_REMU, "remu"},
};

const char* RiscVM::RegisterName(const uint32_t reg)
{
    return RegisterName(static_cast<Register>(reg));
}

const char* RiscVM::RegisterName(const Register reg)
{
    return register_to_string[reg];
}

bool RiscVM::IsRegister(const std::string& name)
{
    return string_to_register.contains(name);
}

RiscVM::Register RiscVM::GetRegister(const std::string& name)
{
    return string_to_register[name];
}

const char* RiscVM::InstructionName(const uint32_t data)
{
    return ISAName(ISA(data));
}

const char* RiscVM::ISAName(const uint32_t isa)
{
    return isa_to_string[isa];
}

bool RiscVM::IsInstruction(const std::string& name)
{
    return string_to_isa.contains(name);
}

uint32_t RiscVM::ISA(const std::string& name)
{
    return string_to_isa[name];
}

uint32_t RiscVM::ISA(const uint32_t data)
{
    uint32_t rv = 0;
    switch (data & 0b1111111)
    {
    case RV32_64G_OP:
        {
            const Format::R f{.Data = data};
            rv = f.Func7 << 10 | f.Func3 << 7 | f.Opcode;
        }
        break;
    case RV32_64G_OP_IMM:
    case RV32_64G_JALR:
    case RV32_64G_LOAD:
    case RV32_64G_MISC_MEM:
    case RV32_64G_SYSTEM:
        {
            const Format::I f{.Data = data};
            rv = f.Func3 << 7 | f.Opcode;
        }
        break;
    case RV32_64G_STORE:
    case RV32_64G_BRANCH:
        {
            const Format::S f{.Data = data};
            rv = f.Func3 << 7 | f.Opcode;
        }
        break;
    case RV32_64G_LUI:
    case RV32_64G_AUIPC:
    case RV32_64G_JAL:
        {
            const Format::U f{.Data = data};
            rv = f.Opcode;
        }
        break;
    default:
        break;
    }
    return rv;
}

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
        RegisterName(Rs2),
        Immediate(),
        RegisterName(Rs1));
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
    return static_cast<int32_t>(Extend(Imm_31, 1) << 31 | Imm_30_12 << 12);
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
    return Format::I{.Data = data}.Immediate();
}

int32_t RiscVM::ImmediateS(const uint32_t data)
{
    return Format::S{.Data = data}.Immediate();
}

int32_t RiscVM::ImmediateB(const uint32_t data)
{
    return Format::B{.Data = data}.Immediate();
}

int32_t RiscVM::ImmediateU(const uint32_t data)
{
    return Format::U{.Data = data}.Immediate();
}

int32_t RiscVM::ImmediateJ(const uint32_t data)
{
    return Format::J{.Data = data}.Immediate();
}

uint32_t RiscVM::UImmediateI(const uint32_t data)
{
    return Format::I{.Data = data}.UImmediate();
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

static std::unordered_map<std::string, RiscVM::RV32IM> string_to_instruction
{
    {"lui", RiscVM::RV32IM_LUI}, {"auipc", RiscVM::RV32IM_AUIPC}, {"jal", RiscVM::RV32IM_JAL},
    {"jalr", RiscVM::RV32IM_JALR}, {"beq", RiscVM::RV32IM_BEQ}, {"bne", RiscVM::RV32IM_BNE},
    {"blt", RiscVM::RV32IM_BLT}, {"bge", RiscVM::RV32IM_BGE}, {"bltu", RiscVM::RV32IM_BLTU},
    {"bgeu", RiscVM::RV32IM_BGEU}, {"lb", RiscVM::RV32IM_LB}, {"lh", RiscVM::RV32IM_LH}, {"lw", RiscVM::RV32IM_LW},
    {"lbu", RiscVM::RV32IM_LBU}, {"lhu", RiscVM::RV32IM_LHU}, {"sb", RiscVM::RV32IM_SB}, {"sh", RiscVM::RV32IM_SH},
    {"sw", RiscVM::RV32IM_SW}, {"addi", RiscVM::RV32IM_ADDI}, {"slti", RiscVM::RV32IM_SLTI},
    {"sltiu", RiscVM::RV32IM_SLTIU}, {"xori", RiscVM::RV32IM_XORI}, {"ori", RiscVM::RV32IM_ORI},
    {"andi", RiscVM::RV32IM_ANDI}, {"slli", RiscVM::RV32IM_SLLI}, {"srli", RiscVM::RV32IM_SRLI},
    {"srai", RiscVM::RV32IM_SRAI}, {"add", RiscVM::RV32IM_ADD}, {"sub", RiscVM::RV32IM_SUB},
    {"sll", RiscVM::RV32IM_SLL}, {"slt", RiscVM::RV32IM_SLT}, {"sltu", RiscVM::RV32IM_SLTU},
    {"xor", RiscVM::RV32IM_XOR}, {"srl", RiscVM::RV32IM_SRL}, {"sra", RiscVM::RV32IM_SRA}, {"or", RiscVM::RV32IM_OR},
    {"and", RiscVM::RV32IM_AND}, {"fence", RiscVM::RV32IM_FENCE}, {"ecall", RiscVM::RV32IM_ECALL},
    {"ebreak", RiscVM::RV32IM_EBREAK}, {"fencei", RiscVM::RV32IM_FENCEI}, {"csrrw", RiscVM::RV32IM_CSRRW},
    {"csrrs", RiscVM::RV32IM_CSRRS}, {"csrrc", RiscVM::RV32IM_CSRRC}, {"csrrwi", RiscVM::RV32IM_CSRRWI},
    {"csrrsi", RiscVM::RV32IM_CSRRSI}, {"csrrci", RiscVM::RV32IM_CSRRCI}, {"mul", RiscVM::RV32IM_MUL},
    {"mulh", RiscVM::RV32IM_MULH}, {"mulhsu", RiscVM::RV32IM_MULHSU}, {"mulhu", RiscVM::RV32IM_MULHU},
    {"div", RiscVM::RV32IM_DIV}, {"divu", RiscVM::RV32IM_DIVU}, {"rem", RiscVM::RV32IM_REM},
    {"remu", RiscVM::RV32IM_REMU},
};

static std::unordered_map<RiscVM::RV32IM, const char*> instruction_to_string
{
    {RiscVM::RV32IM_LUI, "lui"}, {RiscVM::RV32IM_AUIPC, "auipc"}, {RiscVM::RV32IM_JAL, "jal"},
    {RiscVM::RV32IM_JALR, "jalr"}, {RiscVM::RV32IM_BEQ, "beq"}, {RiscVM::RV32IM_BNE, "bne"},
    {RiscVM::RV32IM_BLT, "blt"}, {RiscVM::RV32IM_BGE, "bge"}, {RiscVM::RV32IM_BLTU, "bltu"},
    {RiscVM::RV32IM_BGEU, "bgeu"}, {RiscVM::RV32IM_LB, "lb"}, {RiscVM::RV32IM_LH, "lh"}, {RiscVM::RV32IM_LW, "lw"},
    {RiscVM::RV32IM_LBU, "lbu"}, {RiscVM::RV32IM_LHU, "lhu"}, {RiscVM::RV32IM_SB, "sb"}, {RiscVM::RV32IM_SH, "sh"},
    {RiscVM::RV32IM_SW, "sw"}, {RiscVM::RV32IM_ADDI, "addi"}, {RiscVM::RV32IM_SLTI, "slti"},
    {RiscVM::RV32IM_SLTIU, "sltiu"}, {RiscVM::RV32IM_XORI, "xori"}, {RiscVM::RV32IM_ORI, "ori"},
    {RiscVM::RV32IM_ANDI, "andi"}, {RiscVM::RV32IM_SLLI, "slli"}, {RiscVM::RV32IM_SRLI, "srli"},
    {RiscVM::RV32IM_SRAI, "srai"}, {RiscVM::RV32IM_ADD, "add"}, {RiscVM::RV32IM_SUB, "sub"},
    {RiscVM::RV32IM_SLL, "sll"}, {RiscVM::RV32IM_SLT, "slt"}, {RiscVM::RV32IM_SLTU, "sltu"},
    {RiscVM::RV32IM_XOR, "xor"}, {RiscVM::RV32IM_SRL, "srl"}, {RiscVM::RV32IM_SRA, "sra"}, {RiscVM::RV32IM_OR, "or"},
    {RiscVM::RV32IM_AND, "and"}, {RiscVM::RV32IM_FENCE, "fence"}, {RiscVM::RV32IM_ECALL, "ecall"},
    {RiscVM::RV32IM_EBREAK, "ebreak"}, {RiscVM::RV32IM_FENCEI, "fencei"}, {RiscVM::RV32IM_CSRRW, "csrrw"},
    {RiscVM::RV32IM_CSRRS, "csrrs"}, {RiscVM::RV32IM_CSRRC, "csrrc"}, {RiscVM::RV32IM_CSRRWI, "csrrwi"},
    {RiscVM::RV32IM_CSRRSI, "csrrsi"}, {RiscVM::RV32IM_CSRRCI, "csrrci"}, {RiscVM::RV32IM_MUL, "mul"},
    {RiscVM::RV32IM_MULH, "mulh"}, {RiscVM::RV32IM_MULHSU, "mulhsu"}, {RiscVM::RV32IM_MULHU, "mulhu"},
    {RiscVM::RV32IM_DIV, "div"}, {RiscVM::RV32IM_DIVU, "divu"}, {RiscVM::RV32IM_REM, "rem"},
    {RiscVM::RV32IM_REMU, "remu"},
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
    RV32IM rv;
    switch (data & 0b1111111)
    {
    case RV32_64G_OP:
        {
            const Format::R f{.Data = data};
            rv = static_cast<RV32IM>(f.Func7 << 10 | f.Func3 << 7 | f.Opcode);
        }
        break;
    case RV32_64G_OP_IMM:
    case RV32_64G_JALR:
    case RV32_64G_LOAD:
    case RV32_64G_MISC_MEM:
    case RV32_64G_SYSTEM:
        {
            const Format::I f{.Data = data};
            rv = static_cast<RV32IM>(f.Func3 << 7 | f.Opcode);
        }
        break;
    case RV32_64G_STORE:
    case RV32_64G_BRANCH:
        {
            const Format::S f{.Data = data};
            rv = static_cast<RV32IM>(f.Func3 << 7 | f.Opcode);
        }
        break;
    case RV32_64G_LUI:
    case RV32_64G_AUIPC:
    case RV32_64G_JAL:
        {
            const Format::U f{.Data = data};
            rv = static_cast<RV32IM>(f.Opcode);
        }
        break;
    default:
        return nullptr;
    }

    return InstructionName(rv);
}

const char* RiscVM::InstructionName(const RV32IM instruction)
{
    return instruction_to_string[instruction];
}

bool RiscVM::IsInstruction(const std::string& name)
{
    return string_to_instruction.contains(name);
}

RiscVM::RV32IM RiscVM::GetInstruction(const std::string& name)
{
    return string_to_instruction[name];
}

RiscVM::RV32IM RiscVM::GetInstruction(const uint32_t data)
{
    auto rv = static_cast<RV32IM>(0xffffffff);
    switch (data & 0b1111111)
    {
    case RV32_64G_OP:
        {
            const Format::R f{.Data = data};
            rv = static_cast<RV32IM>(f.Func7 << 10 | f.Func3 << 7 | f.Opcode);
        }
        break;
    case RV32_64G_OP_IMM:
    case RV32_64G_JALR:
    case RV32_64G_LOAD:
    case RV32_64G_MISC_MEM:
    case RV32_64G_SYSTEM:
        {
            const Format::I f{.Data = data};
            rv = static_cast<RV32IM>(f.Func3 << 7 | f.Opcode);
        }
        break;
    case RV32_64G_STORE:
    case RV32_64G_BRANCH:
        {
            const Format::S f{.Data = data};
            rv = static_cast<RV32IM>(f.Func3 << 7 | f.Opcode);
        }
        break;
    case RV32_64G_LUI:
    case RV32_64G_AUIPC:
    case RV32_64G_JAL:
        {
            const Format::U f{.Data = data};
            rv = static_cast<RV32IM>(f.Opcode);
        }
        break;
    default:
        break;
    }
    return rv;
}

#include <cstring>
#include <iostream>
#include <random>
#include <RiscVM/ISA.hpp>
#include <RiscVM/RiscVM.hpp>
#include <RiscVM/VM.hpp>

void RiscVM::VM::Reset()
{
    m_PC = 0;
    m_DirtyPC = false;
    m_Ok = true;
}

void RiscVM::VM::Load(const char* pgm, const size_t len)
{
    if (m_MemorySize < len)
    {
        m_MemorySize = len;
        m_Memory = static_cast<char*>(realloc(m_Memory, m_MemorySize));
    }
    memcpy(m_Memory, pgm, len);
}

bool RiscVM::VM::Cycle()
{
    if (m_Ok && m_PC >= 0 && m_PC < m_MemorySize)
    {
        const auto inst = *reinterpret_cast<uint32_t*>(&m_Memory[m_PC]);
        Exec(inst);
        if (!m_DirtyPC)
            m_PC += 4;
        else m_DirtyPC = false;
        return true;
    }

    return m_Ok = false;
}

char* RiscVM::VM::Memory() const
{
    return m_Memory;
}

size_t RiscVM::VM::MemorySize() const
{
    return m_MemorySize;
}

bool& RiscVM::VM::Ok()
{
    return m_Ok;
}

int32_t& RiscVM::VM::Status()
{
    return m_Status;
}

int32_t& RiscVM::VM::R(const uint32_t r)
{
    if (r == 0)
        return m_Registers[0] = 0;
    return m_Registers[r];
}

std::map<int, RiscVM::ECall>& RiscVM::VM::ECallMap()
{
    return m_ECallMap;
}

void RiscVM::VM::Exec(const uint32_t data)
{
    switch (ISA(data))
    {
    case RV32I_LUI: return LUI(Rd(data), ImmediateU(data));
    case RV32I_AUIPC: return AUIPC(Rd(data), ImmediateU(data));
    case RV32I_JAL: return JAL(Rd(data), ImmediateJ(data));
    case RV32I_JALR: return JALR(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_BEQ: return BEQ(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32I_BNE: return BNE(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32I_BLT: return BLT(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32I_BGE: return BGE(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32I_BLTU: return BLTU(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32I_BGEU: return BGEU(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32I_LB: return LB(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_LH: return LH(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_LW: return LW(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_LBU: return LBU(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_LHU: return LHU(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_SB: return SB(Rs1(data), Rs2(data), ImmediateS(data));
    case RV32I_SH: return SH(Rs1(data), Rs2(data), ImmediateS(data));
    case RV32I_SW: return SW(Rs1(data), Rs2(data), ImmediateS(data));
    case RV32I_ADDI: return ADDI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_SLTI: return SLTI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_SLTIU: return SLTIU(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_XORI: return XORI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_ORI: return ORI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_ANDI: return ANDI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32I_SLLI: return SLLI(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SRLI: return SRLI(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SRAI: return SRAI(Rd(data), Rs1(data), Rs2(data));
    case RV32I_ADD: return ADD(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SUB: return SUB(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SLL: return SLL(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SLT: return SLT(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SLTU: return SLTU(Rd(data), Rs1(data), Rs2(data));
    case RV32I_XOR: return XOR(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SRL: return SRL(Rd(data), Rs1(data), Rs2(data));
    case RV32I_SRA: return SRA(Rd(data), Rs1(data), Rs2(data));
    case RV32I_OR: return OR(Rd(data), Rs1(data), Rs2(data));
    case RV32I_AND: return AND(Rd(data), Rs1(data), Rs2(data));
    case RV32I_ECALL: return ECALL();
    case RV32I_EBREAK: return EBREAK();
    case RV32I_FENCE: return FENCE(Rd(data), Rs1(data), ImmediateI(data));

    case RV32M_MUL: return MUL(Rd(data), Rs1(data), Rs2(data));
    case RV32M_MULH: return MULH(Rd(data), Rs1(data), Rs2(data));
    case RV32M_MULHSU: return MULHSU(Rd(data), Rs1(data), Rs2(data));
    case RV32M_MULHU: return MULHU(Rd(data), Rs1(data), Rs2(data));
    case RV32M_DIV: return DIV(Rd(data), Rs1(data), Rs2(data));
    case RV32M_DIVU: return DIVU(Rd(data), Rs1(data), Rs2(data));
    case RV32M_REM: return REM(Rd(data), Rs1(data), Rs2(data));
    case RV32M_REMU: return REMU(Rd(data), Rs1(data), Rs2(data));

    default: throw std::runtime_error("no such opcode");
    }
}

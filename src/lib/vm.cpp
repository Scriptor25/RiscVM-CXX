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

char* RiscVM::VM::Resize(const size_t size)
{
    const auto old_size = m_MemorySize;
    m_MemorySize += size;
    m_Memory = static_cast<char*>(realloc(m_Memory, m_MemorySize));
    memset(m_Memory + old_size, 0, size);
    return m_Memory + old_size;
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

int32_t RiscVM::VM::Status() const
{
    return m_Status;
}

int32_t& RiscVM::VM::R(const uint32_t r)
{
    if (r == 0)
        return m_Registers[0] = 0;
    return m_Registers[r];
}

void RiscVM::VM::Exec(const uint32_t data)
{
    switch (GetInstruction(data))
    {
    case RV32IM_LUI: return LUI(Rd(data), ImmediateU(data));
    case RV32IM_AUIPC: return AUIPC(Rd(data), ImmediateU(data));
    case RV32IM_JAL: return JAL(Rd(data), ImmediateJ(data));
    case RV32IM_JALR: return JALR(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_BEQ: return BEQ(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32IM_BNE: return BNE(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32IM_BLT: return BLT(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32IM_BGE: return BGE(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32IM_BLTU: return BLTU(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32IM_BGEU: return BGEU(Rs1(data), Rs2(data), ImmediateB(data));
    case RV32IM_LB: return LB(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_LH: return LH(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_LW: return LW(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_LBU: return LBU(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_LHU: return LHU(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_SB: return SB(Rs1(data), Rs2(data), ImmediateS(data));
    case RV32IM_SH: return SH(Rs1(data), Rs2(data), ImmediateS(data));
    case RV32IM_SW: return SW(Rs1(data), Rs2(data), ImmediateS(data));
    case RV32IM_ADDI: return ADDI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_SLTI: return SLTI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_SLTIU: return SLTIU(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_XORI: return XORI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_ORI: return ORI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_ANDI: return ANDI(Rd(data), Rs1(data), ImmediateI(data));
    case RV32IM_SLLI: return SLLI(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SRLI: return SRLI(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SRAI: return SRAI(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_ADD: return ADD(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SUB: return SUB(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SLL: return SLL(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SLT: return SLT(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SLTU: return SLTU(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_XOR: return XOR(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SRL: return SRL(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_SRA: return SRA(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_OR: return OR(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_AND: return AND(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_ECALL: return ECALL();
    case RV32IM_EBREAK: return EBREAK();
    case RV32IM_FENCE: return FENCE(Rd(data), Rs1(data), ImmediateI(data));

    case RV32IM_MUL: return MUL(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_MULH: return MULH(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_MULHSU: return MULHSU(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_MULHU: return MULHU(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_DIV: return DIV(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_DIVU: return DIVU(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_REM: return REM(Rd(data), Rs1(data), Rs2(data));
    case RV32IM_REMU: return REMU(Rd(data), Rs1(data), Rs2(data));

    default: throw std::runtime_error("no such opcode");
    }
}

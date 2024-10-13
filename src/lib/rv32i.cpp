#include <iostream>
#include <random>
#include <RiscVM/ISA.hpp>
#include <RiscVM/VM.hpp>

void RiscVM::VM::LUI(const uint32_t rd, const int32_t imm)
{
    R(rd) = imm;
}

void RiscVM::VM::AUIPC(const uint32_t rd, const int32_t imm)
{
    R(rd) = imm + m_PC;
}

void RiscVM::VM::JAL(const uint32_t rd, const int32_t imm)
{
    R(rd) = m_PC + 4;
    m_PC += imm;
    m_DirtyPC = true;
}

void RiscVM::VM::JALR(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    const auto a = R(rs1) + imm;
    R(rd) = m_PC + 4;
    m_PC = a;
    m_DirtyPC = true;
}

void RiscVM::VM::BEQ(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    if (R(rs1) == R(rs2))
    {
        m_PC += imm;
        m_DirtyPC = true;
    }
}

void RiscVM::VM::BNE(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    if (R(rs1) != R(rs2))
    {
        m_PC += imm;
        m_DirtyPC = true;
    }
}

void RiscVM::VM::BLT(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    if (R(rs1) < R(rs2))
    {
        m_PC += imm;
        m_DirtyPC = true;
    }
}

void RiscVM::VM::BGE(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    if (R(rs1) >= R(rs2))
    {
        m_PC += imm;
        m_DirtyPC = true;
    }
}

void RiscVM::VM::BLTU(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    if (static_cast<uint32_t>(R(rs1)) < static_cast<uint32_t>(R(rs2)))
    {
        m_PC += imm;
        m_DirtyPC = true;
    }
}

void RiscVM::VM::BGEU(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    if (static_cast<uint32_t>(R(rs1)) >= static_cast<uint32_t>(R(rs2)))
    {
        m_PC += imm;
        m_DirtyPC = true;
    }
}

void RiscVM::VM::LB(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = static_cast<uint8_t>(*reinterpret_cast<int8_t*>(&m_Memory[R(rs1) + imm]));
}

void RiscVM::VM::LH(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = *reinterpret_cast<int16_t*>(&m_Memory[R(rs1) + imm]);
}

void RiscVM::VM::LW(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = *reinterpret_cast<int32_t*>(&m_Memory[R(rs1) + imm]);
}

void RiscVM::VM::LBU(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = *reinterpret_cast<uint8_t*>(&m_Memory[R(rs1) + imm]);
}

void RiscVM::VM::LHU(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = *reinterpret_cast<uint16_t*>(&m_Memory[R(rs1) + imm]);
}

void RiscVM::VM::SB(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    *reinterpret_cast<int8_t*>(&m_Memory[R(rs2) + imm]) = static_cast<int8_t>(R(rs1));
}

void RiscVM::VM::SH(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    *reinterpret_cast<int16_t*>(&m_Memory[R(rs2) + imm]) = static_cast<int16_t>(R(rs1));
}

void RiscVM::VM::SW(const uint32_t rs1, const uint32_t rs2, const int32_t imm)
{
    *reinterpret_cast<int32_t*>(&m_Memory[R(rs2) + imm]) = R(rs1);
}

void RiscVM::VM::ADDI(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = R(rs1) + imm;
}

void RiscVM::VM::SLTI(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = R(rs1) < imm;
}

void RiscVM::VM::SLTIU(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = static_cast<uint32_t>(R(rs1)) < static_cast<uint32_t>(imm);
}

void RiscVM::VM::XORI(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = R(rs1) ^ imm;
}

void RiscVM::VM::ORI(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = R(rs1) | imm;
}

void RiscVM::VM::ANDI(const uint32_t rd, const uint32_t rs1, const int32_t imm)
{
    R(rd) = R(rs1) & imm;
}

void RiscVM::VM::SLLI(const uint32_t rd, const uint32_t rs1, const uint32_t imm)
{
    R(rd) = R(rs1) << imm;
}

void RiscVM::VM::SRLI(const uint32_t rd, const uint32_t rs1, const uint32_t imm)
{
    R(rd) = R(rs1) >> imm;
}

void RiscVM::VM::SRAI(const uint32_t rd, const uint32_t rs1, const uint32_t imm)
{
    R(rd) = R(rs1) >> imm;
}

void RiscVM::VM::ADD(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) + R(rs2);
}

void RiscVM::VM::SUB(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) - R(rs2);
}

void RiscVM::VM::SLL(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) << R(rs2);
}

void RiscVM::VM::SLT(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) < R(rs2);
}

void RiscVM::VM::SLTU(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = static_cast<uint32_t>(R(rs1)) < static_cast<uint32_t>(R(rs2));
}

void RiscVM::VM::XOR(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) ^ R(rs2);
}

void RiscVM::VM::SRL(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) >> R(rs2);
}

void RiscVM::VM::SRA(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) >> R(rs2);
}

void RiscVM::VM::OR(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) | R(rs2);
}

void RiscVM::VM::AND(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) & R(rs2);
}

void RiscVM::VM::FENCE(const uint32_t rd, const uint32_t rs1, const uint32_t fm_pred_succ)
{
    const auto fm = fm_pred_succ >> 8 & 0b1111;
    const auto pred = fm_pred_succ >> 4 & 0b1111;
    const auto succ = fm_pred_succ & 0b1111;

    (void)fm;
    (void)pred;
    (void)succ;

    // TODO: do something
}

void RiscVM::VM::ECALL()
{
    m_ECallMap[R(a7)](*this);
}

void RiscVM::VM::EBREAK()
{
}

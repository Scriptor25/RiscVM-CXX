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

char* RiscVM::VM::ResizeBy(const size_t size)
{
    const auto old_size = m_MemorySize;
    m_MemorySize += size;
    m_Memory = static_cast<char*>(realloc(m_Memory, m_MemorySize));

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

bool RiscVM::VM::Ok() const
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
    case RV32IM_CSRRW: return CSRRW(Rd(data), Rs1(data), UImmediateI(data));
    case RV32IM_CSRRS: return CSRRS(Rd(data), Rs1(data), UImmediateI(data));
    case RV32IM_CSRRC: return CSRRC(Rd(data), Rs1(data), UImmediateI(data));
    case RV32IM_CSRRWI: return CSRRWI(Rd(data), Rs1(data), UImmediateI(data));
    case RV32IM_CSRRSI: return CSRRSI(Rd(data), Rs1(data), UImmediateI(data));
    case RV32IM_CSRRCI: return CSRRCI(Rd(data), Rs1(data), UImmediateI(data));
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

void RiscVM::VM::FENCE(uint32_t rd, uint32_t rs1, uint32_t fm_pred_succ)
{
    std::cerr << "not yet implemented: fence" << std::endl;
}

void RiscVM::VM::ECALL()
{
    switch (R(a7))
    {
    case 0: // putc
        fputc(R(a0), stdout);
        fflush(stdout);
        break;
    case 1: // puts
        fputs(&m_Memory[R(a0)], stdout);
        fflush(stdout);
        break;
    case 2: // printf
        vprintf(&m_Memory[R(a0)], &m_Memory[R(a1)]);
        fflush(stdout);
        break;
    case 3: // getc
        R(a0) = fgetc(stdin);
        break;
    case 4: // gets
        fgets(&m_Memory[R(a0)], R(a1), stdin);
        break;
    case 5: // scanf
        vfscanf(stdin, &m_Memory[R(a0)], &m_Memory[R(a1)]);
        break;
    case 120: // random
        {
            const auto t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            R(a0) = R(a0) + static_cast<int32_t>(static_cast<float>(R(a1) - R(a0)) * t);
        }
        break;
    case 127:
        m_Ok = false;
        m_Status = R(a0);
        break;
    default:
        break;
    }
}

void RiscVM::VM::EBREAK()
{
    std::cerr << "not yet implemented: ebreak" << std::endl;
}

void RiscVM::VM::CSRRW(uint32_t rd, uint32_t rs1, uint32_t csr)
{
    std::cerr << "not yet implemented: csrrw" << std::endl;
}

void RiscVM::VM::CSRRS(uint32_t rd, uint32_t rs1, uint32_t csr)
{
    std::cerr << "not yet implemented: csrrs" << std::endl;
}

void RiscVM::VM::CSRRC(uint32_t rd, uint32_t rs1, uint32_t csr)
{
    std::cerr << "not yet implemented: csrrc" << std::endl;
}

void RiscVM::VM::CSRRWI(uint32_t rd, uint32_t u_imm, uint32_t csr)
{
    std::cerr << "not yet implemented: csrrwi" << std::endl;
}

void RiscVM::VM::CSRRSI(uint32_t rd, uint32_t u_imm, uint32_t csr)
{
    std::cerr << "not yet implemented: csrrsi" << std::endl;
}

void RiscVM::VM::CSRRCI(uint32_t rd, uint32_t u_imm, uint32_t csr)
{
    std::cerr << "not yet implemented: csrrci" << std::endl;
}

void RiscVM::VM::MUL(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) * R(rs2);
}

void RiscVM::VM::MULH(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = static_cast<int16_t>(R(rs1)) * static_cast<int16_t>(R(rs2));
}

void RiscVM::VM::MULHSU(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = static_cast<int16_t>(R(rs1)) * static_cast<uint16_t>(R(rs2));
}

void RiscVM::VM::MULHU(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = static_cast<uint16_t>(R(rs1)) * static_cast<uint16_t>(R(rs2));
}

void RiscVM::VM::DIV(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) / R(rs2);
}

void RiscVM::VM::DIVU(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = static_cast<int32_t>(static_cast<uint32_t>(R(rs1)) / static_cast<uint32_t>(R(rs2)));
}

void RiscVM::VM::REM(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = R(rs1) % R(rs2);
}

void RiscVM::VM::REMU(const uint32_t rd, const uint32_t rs1, const uint32_t rs2)
{
    R(rd) = static_cast<int32_t>(static_cast<uint32_t>(R(rs1)) % static_cast<uint32_t>(R(rs2)));
}

#include <RiscVM/VM.hpp>

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

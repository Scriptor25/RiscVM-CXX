#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParseNoOp(const std::string& name, std::vector<OperandPtr>& operands) const
{
    // no-op: nop
    // addi zero,zero,0
    if (name == "nop" && operands.empty())
    {
        operands.push_back(Reg(zero));
        operands.push_back(Reg(zero));
        operands.push_back(Imm(0));
        m_ActiveSection->EmplaceBack(RV32IM_ADDI, operands);
        return true;
    }

    return false;
}

#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParseMove(const std::string& name, std::vector<OperandPtr>& operands) const
{
    // move: mv rd,rs1
    // addi rd,rs1,0
    if (name == "mv" && operands.size() == 2)
    {
        operands.push_back(Imm(0));
        m_ActiveSection->EmplaceBack(RV32I_ADDI, operands);

        return true;
    }

    return false;
}

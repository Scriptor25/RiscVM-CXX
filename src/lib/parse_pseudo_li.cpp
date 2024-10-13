#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParsePseudoLI(const std::string& name, std::vector<OperandPtr>& operands) const
{
    // load immediate: li rd,imm
    // lui  rd,imm[31:12]+imm[11]
    // addi rd,rd,imm[11:0]
    if (name == "li" && operands.size() == 2)
    {
        const auto rd = operands[0];
        const auto imm = operands[1];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(imm, 31, 12, false), Bits(imm, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_LUI, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(rd);
        operands.push_back(Bits(imm, 11, 0, true));
        m_ActiveSection->EmplaceBack(RV32IM_ADDI, operands);

        return true;
    }

    return false;
}

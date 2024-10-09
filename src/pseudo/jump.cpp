#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParsePseudoJump(
    const std::string& name,
    std::vector<OperandPtr>& operands) const
{
    // jump: j sym
    // jal zero,sym
    if (name == "j" && operands.size() == 1)
    {
        const auto sym = operands[0];

        operands.clear();
        operands.push_back(Reg(zero));
        operands.push_back(sym);
        m_ActiveSection->EmplaceBack(RV32IM_JAL, operands);

        return true;
    }

    // jump and link: jal sym
    // jal ra,sym
    if (name == "jal" && operands.size() == 1)
    {
        const auto sym = operands[0];

        operands.clear();
        operands.push_back(Reg(ra));
        operands.push_back(sym);
        m_ActiveSection->EmplaceBack(RV32IM_JAL, operands);

        return true;
    }

    // jump register: jr rs1
    // jalr zero,0(rs1)
    if (name == "jr" && operands.size() == 1)
    {
        const auto rs1 = operands[0];

        operands.clear();
        operands.push_back(Reg(zero));
        operands.push_back(Off(Imm(0), rs1));
        m_ActiveSection->EmplaceBack(RV32IM_JALR, operands);

        return true;
    }

    // jump and link register: jalr rs1
    // jalr ra,0(rs1)
    if (name == "jalr" && operands.size() == 1)
    {
        const auto rs1 = operands[0];

        operands.clear();
        operands.push_back(Reg(ra));
        operands.push_back(Off(Imm(0), rs1));
        m_ActiveSection->EmplaceBack(RV32IM_JALR, operands);

        return true;
    }

    return false;
}

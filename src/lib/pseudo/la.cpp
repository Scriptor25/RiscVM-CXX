#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParsePseudoLA(
    const std::string& name,
    std::vector<OperandPtr>& operands) const
{
    // load address: la rd,sym
    // auipc rd,(sym-pc)[31:12]+(sym-pc)[11]
    // addi  rd,rd,(sym-pc)[11:0]
    if (name == "la" && operands.size() == 2)
    {
        const auto rd = operands[0];
        const auto sym = operands[1];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(rd);
        operands.push_back(Bits(sym, 11, 0, true));
        m_ActiveSection->EmplaceBack(RV32IM_ADDI, operands);

        return true;
    }

    return false;
}

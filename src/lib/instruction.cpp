#include <stdexcept>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/ISA.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>

void RiscVM::Assembler::ParseInstruction()
{
    const auto name = Expect(TokenType_Symbol).Value;

    std::vector<OperandPtr> operands;
    if (!At(TokenType_EOF) && !At(TokenType_NewLine))
        do operands.push_back(ParseOperand());
        while (NextAt(TokenType_Comma));

    if (ParsePseudoInstruction(name, operands))
        return;

    const auto rv = GetInstruction(name);
    if (!rv)
        throw std::runtime_error("no such instruction");

    m_ActiveSection->EmplaceBack(rv, operands);
}

bool RiscVM::Assembler::ParsePseudoInstruction(const std::string& name, std::vector<OperandPtr>& operands) const
{
    if (ParsePseudoLA(name, operands)) return true;
    if (ParsePseudoLoad(name, operands)) return true;
    if (ParsePseudoStore(name, operands)) return true;
    if (ParsePseudoNOP(name, operands)) return true;
    if (ParsePseudoLI(name, operands)) return true;
    if (ParsePseudoMV(name, operands)) return true;
    if (ParsePseudoJump(name, operands)) return true;
    if (ParsePseudoCall(name, operands)) return true;

    return false;
}

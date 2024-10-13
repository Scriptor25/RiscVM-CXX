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

    if (ParsePseudo(name, operands))
        return;

    const auto rv = GetInstruction(name);
    if (!rv)
        throw std::runtime_error("no such instruction");

    m_ActiveSection->EmplaceBack(rv, operands);
}

bool RiscVM::Assembler::ParsePseudo(const std::string& name, std::vector<OperandPtr>& operands) const
{
    if (ParseLoad(name, operands)) return true;
    if (ParseStore(name, operands)) return true;
    if (ParseNoOp(name, operands)) return true;
    if (ParseMove(name, operands)) return true;
    if (ParseJump(name, operands)) return true;
    if (ParseCall(name, operands)) return true;

    return false;
}

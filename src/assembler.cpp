#include <istream>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>

std::ostream& RiscVM::operator<<(std::ostream& os, const TokenType& type)
{
    switch (type)
    {
    case TokenType_EOF: return os << "EOF";
    case TokenType_NewLine: return os << "New Line";
    case TokenType_Label: return os << "Label";
    case TokenType_CompileDirective: return os << "Compile Directive";
    case TokenType_Symbol: return os << "Symbol";
    case TokenType_Immediate: return os << "Immediate";
    case TokenType_Char: return os << "Char";
    case TokenType_Dot: return os << "Dot";
    case TokenType_Comma: return os << "Comma";
    default: return os << "?";
    }
}

std::ostream& RiscVM::operator<<(std::ostream& os, const Token& token)
{
    return os << token.Type << ": '" << token.Value << "' | " << token.Immediate;
}

RiscVM::Assembler::Assembler(std::istream& stream)
    : m_Stream(stream)
{
    m_C = stream.get();
    Next();

    m_ActiveSection = &m_Sections[".text"];
}

void RiscVM::Assembler::Parse()
{
    while (NextAt(TokenType_NewLine))
    {
    }

    if (At(TokenType_Label))
        ParseLabel();
    else if (At(TokenType_RelativeLabel))
        ParseRelativeLabel();

    if (At(TokenType_Symbol))
    {
        if (m_Token.Value.front() == '.')
        {
            ParseCompileDirective();
        }
        else
        {
            ParseInstruction();
        }
    }

    if (!At(TokenType_EOF))
        Expect(TokenType_NewLine);
}

void RiscVM::Assembler::ParseLabel()
{
    const auto label = Expect(TokenType_Label).Value;
    auto& [base, offset, global] = m_SymbolTable[label];
    base = m_ActiveSection;
    offset = m_ActiveSection->Size();

    m_RelativeBase = &m_SymbolTable[label];
}

void RiscVM::Assembler::ParseRelativeLabel()
{
    const auto label = Expect(TokenType_RelativeLabel).Immediate;
    auto& [base, offset, global] = m_RelativeSymbolTable[reinterpret_cast<intptr_t>(m_RelativeBase) + label];
    base = m_ActiveSection;
    offset = m_ActiveSection->Size();
}

int RiscVM::Assembler::Get() const
{
    return m_Stream.get();
}

RiscVM::Token RiscVM::Assembler::Skip()
{
    auto t = m_Token;
    Next();
    return t;
}

RiscVM::Token RiscVM::Assembler::Expect(const TokenType type)
{
    if (At(type)) return Skip();
    throw std::runtime_error("unexpected token");
}

bool RiscVM::Assembler::At(const TokenType type) const
{
    return type == m_Token.Type;
}

bool RiscVM::Assembler::NextAt(const TokenType type)
{
    if (At(type))
    {
        Next();
        return true;
    }
    return false;
}

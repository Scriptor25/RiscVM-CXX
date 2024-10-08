#pragma once

#include <map>
#include <vector>
#include <RiscVM/RiscVM.hpp>
#include <RiscVM/Section.hpp>
#include <RiscVM/Symbol.hpp>

namespace RiscVM
{
    enum TokenType
    {
        TokenType_EOF = -1,
        TokenType_NewLine,
        TokenType_Label,
        TokenType_RelativeLabel,
        TokenType_CompileDirective,
        TokenType_Symbol,
        TokenType_RelativeSymbol,
        TokenType_Immediate,
        TokenType_Char,

        TokenType_Dot,
        TokenType_Comma,
        TokenType_ParenOpen,
        TokenType_ParenClose,
        TokenType_Minus,
    };

    struct Token
    {
        TokenType Type = TokenType_EOF;
        std::string Value;
        uint32_t Immediate = 0;
    };

    std::ostream& operator<<(std::ostream& os, const TokenType& type);
    std::ostream& operator<<(std::ostream& os, const Token& token);

    class Assembler
    {
    public:
        explicit Assembler(std::istream& stream);

        std::vector<char> Link();

        void Parse();
        void ParseLabel();
        void ParseRelativeLabel();
        void ParseCompileDirective();
        void ParseInstruction();
        bool ParsePseudoInstruction(const std::string& name, std::vector<OperandPtr>& operands) const;

        OperandPtr ParseOperand();

        [[nodiscard]] int Get() const;
        Token& Next();
        Token Skip();
        Token Expect(TokenType);
        [[nodiscard]] bool At(TokenType) const;
        bool NextAt(TokenType);

    private:
        std::istream& m_Stream;
        int m_C;
        Token m_Token;

        Section* m_ActiveSection;

        std::map<std::string, Section> m_Sections;
        std::map<std::string, Symbol> m_SymbolTable;

        Symbol* m_RelativeBase{};
        std::map<uint32_t, Symbol> m_RelativeSymbolTable;
    };
}

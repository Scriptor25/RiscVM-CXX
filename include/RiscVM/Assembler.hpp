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
        TokenType_CompileDirective,
        TokenType_Symbol,
        TokenType_Immediate,
        TokenType_Char,

        TokenType_Dot,
        TokenType_Comma,
        TokenType_ParenOpen,
        TokenType_ParenClose,
        TokenType_Operator,
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

        std::vector<char> Parse();

    private:
        void ParseLine();
        void ParseLabel();
        void ParseCompileDirective();
        void ParseInstruction();

        bool ParsePseudo(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoLA(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoLoad(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoStore(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoNOP(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoLI(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoMV(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoJump(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParsePseudoCall(const std::string& name, std::vector<OperandPtr>& operands) const;

        OperandPtr ParseOperand();
        OperandPtr ParsePrimary();
        OperandPtr ParseBinary(OperandPtr lhs, int min_pre);

        std::vector<char> Link();

        [[nodiscard]] int Get() const;
        Token& Next();
        Token Skip();
        Token Expect(TokenType);
        [[nodiscard]] bool At(TokenType) const;
        [[nodiscard]] bool At(const std::string&) const;
        bool NextAt(TokenType);
        bool NextAt(const std::string&);

    private:
        std::istream& m_Stream;
        int m_C;
        Token m_Token;

        Section* m_ActiveSection;

        std::map<std::string, Section> m_Sections;
        std::map<std::string, Symbol> m_SymbolTable;

        Symbol* m_RelativeBase{};
    };
}

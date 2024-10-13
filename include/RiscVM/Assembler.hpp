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

    struct SectionLinkInfo
    {
        std::string Name;
        size_t Align = 0;
        size_t Size = 0;
        size_t Offset = 0;
    };

    struct LinkInfo
    {
        std::vector<SectionLinkInfo> Sections;
    };

    class Assembler
    {
    public:
        explicit Assembler(std::istream& stream);

        std::vector<char> Parse(LinkInfo&);

    private:
        void ParseLine();
        void ParseLabel();
        void ParseCompileDirective();
        void ParseInstruction();

        bool ParsePseudo(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParseLoad(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParseStore(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParseNoOp(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParseMove(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParseJump(const std::string& name, std::vector<OperandPtr>& operands) const;
        bool ParseCall(const std::string& name, std::vector<OperandPtr>& operands) const;

        OperandPtr ParseOperand();
        OperandPtr ParsePrimary();
        OperandPtr ParseBinary(OperandPtr lhs, int min_pre);

        std::vector<char> Link(LinkInfo&);

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

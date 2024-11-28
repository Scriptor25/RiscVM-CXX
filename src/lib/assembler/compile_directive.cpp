#include <stdexcept>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>

void RiscVM::Assembler::ParseCompileDirective()
{
    const auto directive = Expect(TokenType_Symbol).Value;
    if (directive == ".align")
    {
        const auto n = ParseOperand()->AsImmediate();
        const auto align = 1 << n;
        if (const auto rem = m_ActiveSection->Size() % align)
            m_ActiveSection->Skip(align - rem);
        return;
    }
    if (directive == ".ascii")
    {
        do
            if (At(TokenType_Immediate))
                m_ActiveSection->Data.push_back(static_cast<char>(Skip().Immediate));
            else
                for (auto str = Expect(TokenType_Char).Value; const auto c : str)
                    m_ActiveSection->Data.push_back(c);
        while (NextAt(TokenType_Comma));
        return;
    }
    if (directive == ".globl")
    {
        const auto label = Expect(TokenType_Symbol).Value;
        m_SymbolTable[label].Global = true;
        return;
    }
    if (directive == ".section")
    {
        const auto section = Expect(TokenType_Symbol).Value;
        m_ActiveSection = &m_Sections[section];
        return;
    }
    if (directive == ".set")
    {
        const auto label = Expect(TokenType_Symbol).Value;
        const auto imm = ParseOperand()->AsImmediate();
        auto& symbol = m_SymbolTable[label];
        symbol.Base = reinterpret_cast<Section*>(1);
        symbol.Offset = imm;
        return;
    }
    if (directive == ".skip")
    {
        const auto n = ParseOperand()->AsImmediate();
        m_ActiveSection->Skip(n);
        return;
    }
    if (directive == ".word")
    {
        do m_ActiveSection->PushBack(ParseOperand()->AsImmediate());
        while (NextAt(TokenType_Comma));
        return;
    }

    throw std::runtime_error("no such compile directive");
}

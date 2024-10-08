#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>

void RiscVM::Assembler::ParseCompileDirective()
{
    const auto directive = Expect(TokenType_Symbol).Value;
    if (directive == ".section")
    {
        const auto section = Expect(TokenType_Symbol).Value;
        m_ActiveSection = &m_Sections[section];
        return;
    }
    if (directive == ".globl")
    {
        const auto label = Expect(TokenType_Symbol).Value;
        m_SymbolTable[label].Global = true;
        return;
    }
    if (directive == ".skip")
    {
        const auto n = Expect(TokenType_Immediate).Immediate;
        m_ActiveSection->Skip(n);
        return;
    }
    if (directive == ".align")
    {
        const auto n = Expect(TokenType_Immediate).Immediate;
        if (const auto rem = m_ActiveSection->Size() % n)
            m_ActiveSection->Skip(n - rem);
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

    throw std::runtime_error("no such compile directive");
}

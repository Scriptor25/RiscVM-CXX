#include <RiscVM/Assembler.hpp>
#include <RiscVM/Instruction.hpp>
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
    if (directive == ".skip")
    {
        const auto n = Expect(TokenType_Immediate).Immediate;
        m_ActiveSection->Size += n;
        return;
    }
    if (directive == ".align")
    {
        const auto n = Expect(TokenType_Immediate).Immediate;
        if (const auto rem = m_ActiveSection->Size % n)
            m_ActiveSection->Size += n - rem;
        return;
    }

    throw std::runtime_error("no such compile directive");
}

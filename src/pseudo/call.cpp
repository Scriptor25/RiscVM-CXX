#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParsePseudoCall(
    const std::string& name,
    std::vector<OperandPtr>& operands) const
{
    // return: ret
    // jalr zero,0(ra)
    if (name == "ret" && operands.empty())
    {
        operands.push_back(Reg(zero));
        operands.push_back(Off(Imm(0), Reg(ra)));
        m_ActiveSection->EmplaceBack(RV32IM_JALR, operands);

        return true;
    }

    // call: call sym
    // auipc ra,(sym-pc)[31:12]+(sym-pc)[11]
    // jalr  ra,(sym-pc)[11:0](ra)
    if (name == "call" && operands.size() == 1)
    {
        const auto sym = operands[0];

        operands.clear();
        operands.push_back(Reg(ra));
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(Reg(ra));
        operands.push_back(Off(Bits(sym, 11, 0, true), Reg(ra)));
        m_ActiveSection->EmplaceBack(RV32IM_JALR, operands);

        return true;
    }

    // tail: tail sym
    // auipc t1,(sym-pc)[31:12]+(sym-pc)[11]
    // jalr  zero,(sym-pc)[11:0](t1)
    if (name == "tail" && operands.size() == 1)
    {
        const auto sym = operands[0];

        operands.clear();
        operands.push_back(Reg(t1));
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(Reg(zero));
        operands.push_back(Off(Bits(sym, 11, 0, true), Reg(t1)));
        m_ActiveSection->EmplaceBack(RV32IM_JALR, operands);

        return true;
    }

    // push: push rs1,rs2,...,rsN
    // addi sp,sp,-4*N
    // sw   rsI,4*(N-I)(sp)
    if (name == "push")
    {
        const auto regs = operands;
        const auto n = static_cast<int32_t>(regs.size());

        operands.clear();
        operands.push_back(Reg(sp));
        operands.push_back(Reg(sp));
        operands.push_back(Imm(-4 * n));
        m_ActiveSection->EmplaceBack(RV32IM_ADDI, operands);

        for (int32_t i = 0; i < regs.size(); ++i)
        {
            const auto& rs = regs[i];

            operands.clear();
            operands.push_back(rs);
            operands.push_back(Off(Imm(4 * (n - i)), Reg(sp)));
            m_ActiveSection->EmplaceBack(RV32IM_SW, operands);
        }

        return true;
    }

    // pop: pop rd1,rd2,...,rdN
    // addi sp,sp,4*N
    // lw   rdI,-4*(N-I-1)(sp)
    if (name == "pop")
    {
        const auto regs = operands;
        const auto n = static_cast<int32_t>(regs.size());

        operands.clear();
        operands.push_back(Reg(sp));
        operands.push_back(Reg(sp));
        operands.push_back(Imm(4 * n));
        m_ActiveSection->EmplaceBack(RV32IM_ADDI, operands);

        for (int32_t i = 0; i < regs.size(); ++i)
        {
            const auto& rd = regs[i];

            operands.clear();
            operands.push_back(rd);
            operands.push_back(Off(Imm(-4 * (n - i - 1)), Reg(sp)));
            m_ActiveSection->EmplaceBack(RV32IM_LW, operands);
        }

        return true;
    }

    return false;
}

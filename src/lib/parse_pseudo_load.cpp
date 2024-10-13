#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParsePseudoLoad(const std::string& name, std::vector<OperandPtr>& operands) const
{
    // load global: lX rd,sym,rt
    // auipc rt,(sym-pc)[31:12]+(sym-pc)[11]
    // lX    rd,(sym-pc)[11:0](rt)
    if (name == "lb" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_LB, operands);

        return true;
    }
    if (name == "lh" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_LH, operands);

        return true;
    }
    if (name == "lw" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_LW, operands);

        return true;
    }
    if (name == "lbu" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_LBU, operands);

        return true;
    }
    if (name == "lhu" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_LHU, operands);

        return true;
    }

    return false;
}

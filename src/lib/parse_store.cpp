#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParseStore(const std::string& name, std::vector<OperandPtr>& operands) const
{
    // store global: sX rs,sym,rt
    // auipc rt,(sym-pc)[31:12]+(sym-pc)[11]
    // sX    rs,(sym-pc)[11:0](rt)
    if (name == "sb" && operands.size() == 3)
    {
        const auto rs = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rs);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_SB, operands);

        return true;
    }
    if (name == "sh" && operands.size() == 3)
    {
        const auto rs = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rs);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_SH, operands);

        return true;
    }
    if (name == "sw" && operands.size() == 3)
    {
        const auto rs = operands[0];
        const auto sym = operands[1];
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rs);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32IM_SW, operands);

        return true;
    }

    return false;
}

#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>

bool RiscVM::Assembler::ParseLoad(const std::string& name, std::vector<OperandPtr>& operands) const
{
    // load address: la rd,sym
    // auipc rd,(sym-pc)[31:12]+(sym-pc)[11]
    // addi  rd,rd,(sym-pc)[11:0]
    if (name == "la" && operands.size() == 2)
    {
        const auto rd = operands[0];
        const auto sym = operands[1];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32I_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(rd);
        operands.push_back(Bits(sym, 11, 0, true));
        m_ActiveSection->EmplaceBack(RV32I_ADDI, operands);

        return true;
    }

    // load immediate: li rd,imm
    // lui  rd,imm[31:12]+imm[11]
    // addi rd,rd,imm[11:0]
    if (name == "li" && operands.size() == 2)
    {
        const auto rd = operands[0];
        const auto imm = operands[1];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(imm, 31, 12, false), Bits(imm, 11, 11, false)));
        m_ActiveSection->EmplaceBack(RV32I_LUI, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(rd);
        operands.push_back(Bits(imm, 11, 0, true));
        m_ActiveSection->EmplaceBack(RV32I_ADDI, operands);

        return true;
    }

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
        m_ActiveSection->EmplaceBack(RV32I_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32I_LB, operands);

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
        m_ActiveSection->EmplaceBack(RV32I_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32I_LH, operands);

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
        m_ActiveSection->EmplaceBack(RV32I_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32I_LW, operands);

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
        m_ActiveSection->EmplaceBack(RV32I_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32I_LBU, operands);

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
        m_ActiveSection->EmplaceBack(RV32I_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(RV32I_LHU, operands);

        return true;
    }

    return false;
}

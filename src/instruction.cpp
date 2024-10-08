#include <RiscVM/Assembler.hpp>
#include <RiscVM/ISA.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>

void RiscVM::Assembler::ParseInstruction()
{
    const auto name = Expect(TokenType_Symbol).Value;

    std::vector<OperandPtr> operands;
    if (!At(TokenType_EOF) && !At(TokenType_NewLine))
        do operands.push_back(ParseOperand());
        while (NextAt(TokenType_Comma));

    if (ParsePseudoInstruction(name, operands))
        return;

    const auto rv = GetInstruction(name);
    if (!rv)
        throw std::runtime_error("no such instruction");

    const auto offset = m_ActiveSection->Size();
    m_ActiveSection->EmplaceBack(offset, rv, operands);
}

bool RiscVM::Assembler::ParsePseudoInstruction(const std::string& name, std::vector<OperandPtr>& operands) const
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
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(rd);
        operands.push_back(Bits(sym, 11, 0, true));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_ADDI, operands);

        return true;
    }

    // load global: lX rd,sym,rt
    // auipc rt,(sym-pc)[31:12]+(sym-pc)[11]
    // lX    rd,(sym-pc)[11:0](rt)
    if (name == "lb" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_LB, operands);

        return true;
    }
    if (name == "lh" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_LH, operands);

        return true;
    }
    if (name == "lw" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_LW, operands);

        return true;
    }
    if (name == "lbu" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_LBU, operands);

        return true;
    }
    if (name == "lhu" && operands.size() == 3)
    {
        const auto rd = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_LHU, operands);

        return true;
    }

    // store global: sX rs,sym,rt
    // auipc rt,(sym-pc)[31:12]+(sym-pc)[11]
    // sX    rs,(sym-pc)[11:0](rt)
    if (name == "sb" && operands.size() == 3)
    {
        const auto rs = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rs);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_SB, operands);

        return true;
    }
    if (name == "sh" && operands.size() == 3)
    {
        const auto rs = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rs);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_SH, operands);

        return true;
    }
    if (name == "sw" && operands.size() == 3)
    {
        const auto rs = operands[0];
        const auto sym = Sub(operands[1], Imm(4));
        const auto rt = operands[2];

        operands.clear();
        operands.push_back(rt);
        operands.push_back(Add(Bits(sym, 31, 12, false), Bits(sym, 11, 11, false)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(rs);
        operands.push_back(Off(Bits(sym, 11, 0, true), rt));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_SW, operands);

        return true;
    }

    // no-op: nop
    // addi zero,zero,0
    if (name == "nop" && operands.empty())
    {
        operands.push_back(Reg(zero));
        operands.push_back(Reg(zero));
        operands.push_back(Imm(0));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_ADDI, operands);
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
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_LUI, operands);

        operands.clear();
        operands.push_back(rd);
        operands.push_back(rd);
        operands.push_back(Bits(imm, 11, 0, true));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_ADDI, operands);

        return true;
    }

    // move: mv rd,rs1
    // addi rd,rs1,0
    if (name == "mv" && operands.size() == 2)
    {
        operands.push_back(Imm(0));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_ADDI, operands);

        return true;
    }

    // jump: j sym
    // jal zero,sym
    if (name == "j" && operands.size() == 1)
    {
        const auto sym = operands[0];

        operands.clear();
        operands.push_back(Reg(zero));
        operands.push_back(sym);
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_JAL, operands);

        return true;
    }

    // jump and link: jal sym
    // jal ra,sym
    if (name == "jal" && operands.size() == 1)
    {
        const auto sym = operands[0];

        operands.clear();
        operands.push_back(Reg(ra));
        operands.push_back(sym);
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_JAL, operands);

        return true;
    }

    // jump register: jr rs1
    // jalr zero,0(rs1)
    if (name == "jr" && operands.size() == 1)
    {
        const auto rs1 = operands[0];

        operands.clear();
        operands.push_back(Reg(zero));
        operands.push_back(Off(Imm(0), rs1));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_JALR, operands);

        return true;
    }

    // jump and link register: jalr rs1
    // jalr ra,0(rs1)
    if (name == "jalr" && operands.size() == 1)
    {
        const auto rs1 = operands[0];

        operands.clear();
        operands.push_back(Reg(ra));
        operands.push_back(Off(Imm(0), rs1));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_JALR, operands);

        return true;
    }

    // return: ret
    // jalr zero,0(ra)
    if (name == "ret" && operands.empty())
    {
        operands.push_back(Reg(zero));
        operands.push_back(Off(Imm(0), Reg(ra)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_JALR, operands);

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
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(Reg(ra));
        operands.push_back(Off(Bits(sym, 11, 0, true), Reg(ra)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_JALR, operands);

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
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_AUIPC, operands);

        operands.clear();
        operands.push_back(Reg(zero));
        operands.push_back(Off(Bits(sym, 11, 0, true), Reg(t1)));
        m_ActiveSection->EmplaceBack(m_ActiveSection->Size(), RV32IM_JALR, operands);

        return true;
    }

    return false;
}

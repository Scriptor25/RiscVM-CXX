#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>
#include <RiscVM/Symbol.hpp>
#include <utility>

uint32_t RiscVM::Operand::AsRegister() const
{
    throw std::runtime_error("not a register");
}

int32_t RiscVM::Operand::AsImmediate() const
{
    throw std::runtime_error("not a immediate");
}

RiscVM::ImmediateOperand::ImmediateOperand(const int32_t immediate)
    : Immediate(immediate)
{
}

int32_t RiscVM::ImmediateOperand::AsImmediate() const
{
    return Immediate;
}

RiscVM::SymbolOperand::SymbolOperand(Symbol& sym)
    : Sym(sym)
{
}

int32_t RiscVM::SymbolOperand::AsImmediate() const
{
    if (!Sym.Base) return 0;
    return static_cast<int32_t>(Sym.Base->Offset + Sym.Offset);
}

RiscVM::RegisterOperand::RegisterOperand(const Register reg)
    : Reg(reg)
{
}

uint32_t RiscVM::RegisterOperand::AsRegister() const
{
    return Reg;
}

RiscVM::OffsetOperand::OffsetOperand(OperandPtr base, OperandPtr offset)
    : Base(std::move(base)), Offset(std::move(offset))
{
}

RiscVM::BitsOperand::BitsOperand(OperandPtr imm, const uint32_t beg, const uint32_t end, bool sign_ext)
    : Imm(std::move(imm)), Beg(beg), End(end), SignExt(sign_ext)
{
}

int32_t RiscVM::BitsOperand::AsImmediate() const
{
    const auto imm = Imm->AsImmediate();

    if (SignExt)
    {
        int32_t mask = 0;
        for (unsigned i = Beg; i < End; ++i)
            mask = mask << 1 | 0b1;
        return static_cast<int32_t>(Extend(imm >> End & 0b1, 32 - End) << End | imm >> Beg & mask);
    }

    int32_t mask = 0;
    for (unsigned i = Beg; i <= End; ++i)
        mask = mask << 1 | 0b1;

    return imm >> Beg & mask;
}

RiscVM::AddOperand::AddOperand(OperandPtr lhs, OperandPtr rhs)
    : Lhs(std::move(lhs)), Rhs(std::move(rhs))
{
}

int32_t RiscVM::AddOperand::AsImmediate() const
{
    return Lhs->AsImmediate() + Rhs->AsImmediate();
}

RiscVM::SubOperand::SubOperand(OperandPtr lhs, OperandPtr rhs)
    : Lhs(std::move(lhs)), Rhs(std::move(rhs))
{
}

int32_t RiscVM::SubOperand::AsImmediate() const
{
    return Lhs->AsImmediate() - Rhs->AsImmediate();
}

RiscVM::OperandPtr RiscVM::Imm(const int32_t imm)
{
    return std::make_shared<ImmediateOperand>(imm);
}

RiscVM::OperandPtr RiscVM::Sym(Symbol& sym)
{
    return std::make_shared<SymbolOperand>(sym);
}

RiscVM::OperandPtr RiscVM::Reg(const Register reg)
{
    return std::make_shared<RegisterOperand>(reg);
}

RiscVM::OperandPtr RiscVM::Off(const OperandPtr& offset, const OperandPtr& base)
{
    return std::make_shared<OffsetOperand>(base, offset);
}

RiscVM::OperandPtr RiscVM::Bits(const OperandPtr& imm, uint32_t end, uint32_t beg, bool sign_ext)
{
    return std::make_shared<BitsOperand>(imm, beg, end, sign_ext);
}

RiscVM::OperandPtr RiscVM::Add(const OperandPtr& lhs, const OperandPtr& rhs)
{
    return std::make_shared<AddOperand>(lhs, rhs);
}

RiscVM::OperandPtr RiscVM::Sub(const OperandPtr& lhs, const OperandPtr& rhs)
{
    return std::make_shared<SubOperand>(lhs, rhs);
}

RiscVM::OperandPtr RiscVM::Assembler::ParseOperand()
{
    if (At(TokenType_Symbol))
    {
        const auto symbol = Skip().Value;
        if (IsRegister(symbol))
        {
            const auto reg = GetRegister(symbol);
            return Reg(reg);
        }
        return Sub(Sym(m_SymbolTable[symbol]), Imm(static_cast<int32_t>(m_ActiveSection->Size())));
    }

    if (At(TokenType_RelativeSymbol))
    {
        const auto symbol = Skip().Immediate;
        return Sub(
            Sym(m_RelativeSymbolTable[reinterpret_cast<intptr_t>(m_RelativeBase) + symbol]),
            Imm(static_cast<int32_t>(m_ActiveSection->Size())));
    }

    if (At(TokenType_Immediate))
    {
        const auto immediate = Skip().Immediate;
        auto operand = Imm(static_cast<int32_t>(immediate));
        if (NextAt(TokenType_ParenOpen))
        {
            const auto base = ParseOperand();
            Expect(TokenType_ParenClose);
            return Off(operand, base);
        }
        return operand;
    }

    if (At(TokenType_Char))
    {
        const auto c = Skip().Value;
        return Imm(c.front());
    }

    if (NextAt(TokenType_Dot))
        return Imm(0);

    if (NextAt(TokenType_Minus))
    {
        const auto immediate = Skip().Immediate;
        return Imm(-static_cast<int32_t>(immediate));
    }

    throw std::runtime_error("no such operand");
}

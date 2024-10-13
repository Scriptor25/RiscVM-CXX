#include <stdexcept>
#include <unordered_map>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>
#include <RiscVM/Symbol.hpp>

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

RiscVM::SymbolOperand::SymbolOperand(SymbolBase* sym)
    : Sym(sym)
{
}

int32_t RiscVM::SymbolOperand::AsImmediate() const
{
    if (!Sym->Base) throw std::runtime_error("no such symbol");
    return static_cast<int32_t>((reinterpret_cast<intptr_t>(Sym->Base) != 1 ? Sym->Base->Offset : 0) + Sym->Offset);
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

RiscVM::BinOperand::BinOperand(std::string op, OperandPtr lhs, OperandPtr rhs)
    : Op(std::move(op)), Lhs(std::move(lhs)), Rhs(std::move(rhs))
{
}

int32_t RiscVM::BinOperand::AsImmediate() const
{
    if (Op == "+") return Lhs->AsImmediate() + Rhs->AsImmediate();
    if (Op == "-") return Lhs->AsImmediate() - Rhs->AsImmediate();
    if (Op == "*") return Lhs->AsImmediate() * Rhs->AsImmediate();
    if (Op == "/") return Lhs->AsImmediate() / Rhs->AsImmediate();
    if (Op == "%") return Lhs->AsImmediate() % Rhs->AsImmediate();
    if (Op == "&") return Lhs->AsImmediate() & Rhs->AsImmediate();
    if (Op == "|") return Lhs->AsImmediate() | Rhs->AsImmediate();
    if (Op == "&&") return Lhs->AsImmediate() && Rhs->AsImmediate();
    if (Op == "||") return Lhs->AsImmediate() || Rhs->AsImmediate();
    if (Op == "^") return Lhs->AsImmediate() ^ Rhs->AsImmediate();
    if (Op == "==") return Lhs->AsImmediate() == Rhs->AsImmediate();
    if (Op == "!=") return Lhs->AsImmediate() != Rhs->AsImmediate();
    if (Op == "<=") return Lhs->AsImmediate() <= Rhs->AsImmediate();
    if (Op == ">=") return Lhs->AsImmediate() >= Rhs->AsImmediate();
    if (Op == "<") return Lhs->AsImmediate() < Rhs->AsImmediate();
    if (Op == ">") return Lhs->AsImmediate() > Rhs->AsImmediate();
    if (Op == "<<") return Lhs->AsImmediate() << Rhs->AsImmediate();
    if (Op == ">>") return Lhs->AsImmediate() >> Rhs->AsImmediate();
    throw std::runtime_error("no such operator");
}

RiscVM::OperandPtr RiscVM::Imm(const int32_t imm)
{
    return std::make_shared<ImmediateOperand>(imm);
}

RiscVM::OperandPtr RiscVM::Sym(SymbolBase* sym)
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

RiscVM::OperandPtr RiscVM::Bin(const std::string& op, const OperandPtr& lhs, const OperandPtr& rhs)
{
    return std::make_shared<BinOperand>(op, lhs, rhs);
}

RiscVM::OperandPtr RiscVM::Add(const OperandPtr& lhs, const OperandPtr& rhs)
{
    return Bin("+", lhs, rhs);
}

RiscVM::OperandPtr RiscVM::Sub(const OperandPtr& lhs, const OperandPtr& rhs)
{
    return Bin("-", lhs, rhs);
}

RiscVM::OperandPtr RiscVM::Assembler::ParseOperand()
{
    return ParseBinary(ParsePrimary(), 0);
}

RiscVM::OperandPtr RiscVM::Assembler::ParsePrimary()
{
    if (At(TokenType_Symbol))
    {
        const auto symbol = Skip().Value;
        if (IsRegister(symbol))
            return Reg(GetRegister(symbol));

        const auto pc = Imm(static_cast<int32_t>(m_ActiveSection->Size()));

        if (symbol.front() == '.')
            return Sub(Sym(&m_RelativeBase->SubSymbols[symbol]), pc);

        auto& sym = m_SymbolTable[symbol];
        if (reinterpret_cast<intptr_t>(sym.Base) == 1)
            return Sym(&sym);

        return Sub(Sym(&sym), pc);
    }

    if (At(TokenType_Immediate))
    {
        auto imm = Imm(static_cast<int32_t>(Skip().Immediate));
        if (NextAt(TokenType_ParenOpen))
        {
            const auto base = ParseOperand();
            Expect(TokenType_ParenClose);
            return Off(imm, base);
        }
        return imm;
    }

    if (At(TokenType_Char))
        return Imm(Skip().Value.front());

    if (NextAt(TokenType_Dot))
        return Imm(0);

    if (NextAt("-"))
        return Imm(-static_cast<int32_t>(Skip().Immediate));

    if (NextAt(TokenType_ParenOpen))
    {
        auto operand = ParseOperand();
        Expect(TokenType_ParenClose);
        return operand;
    }

    throw std::runtime_error("no such operand");
}

RiscVM::OperandPtr RiscVM::Assembler::ParseBinary(OperandPtr lhs, const int min_pre)
{
    static std::unordered_map<std::string, int> precedences
    {
        {"*", 10}, {"/", 10}, {"%", 10},
        {"+", 9}, {"-", 9},
        {"<<", 8}, {">>", 8},
        {"<", 7}, {"<=", 7}, {">", 7}, {">=", 7},
        {"==", 6}, {"!=", 6},
        {"&", 5},
        {"^", 4},
        {"|", 3},
        {"&&", 2},
        {"||", 1},
    };

    while (At(TokenType_Operator) && precedences[m_Token.Value] >= min_pre)
    {
        const auto op = Skip().Value;
        auto rhs = ParsePrimary();
        while (At(TokenType_Operator) && precedences[m_Token.Value] > precedences[op])
            rhs = ParseBinary(rhs, precedences[op] + 1);
        lhs = Bin(op, lhs, rhs);
    }

    return lhs;
}

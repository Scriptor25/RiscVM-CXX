#pragma once

#include <cstdint>
#include <memory>
#include <RiscVM/ISA.hpp>

namespace RiscVM
{
    struct Operand
    {
        virtual ~Operand() = default;

        [[nodiscard]] virtual uint32_t AsRegister() const;
        [[nodiscard]] virtual int32_t AsImmediate() const;
    };

    struct ImmediateOperand : Operand
    {
        explicit ImmediateOperand(int32_t immediate);

        [[nodiscard]] int32_t AsImmediate() const override;

        int32_t Immediate;
    };

    struct SymbolOperand : Operand
    {
        explicit SymbolOperand(SymbolBase* sym);

        [[nodiscard]] int32_t AsImmediate() const override;

        SymbolBase* Sym;
    };

    struct RegisterOperand : Operand
    {
        explicit RegisterOperand(Register reg);

        [[nodiscard]] uint32_t AsRegister() const override;

        Register Reg;
    };

    struct OffsetOperand : Operand
    {
        OffsetOperand(OperandPtr base, OperandPtr offset);

        OperandPtr Base;
        OperandPtr Offset;
    };

    struct BitsOperand : Operand
    {
        BitsOperand(OperandPtr imm, uint32_t beg, uint32_t end, bool sign_ext);

        [[nodiscard]] int32_t AsImmediate() const override;

        OperandPtr Imm;
        uint32_t Beg;
        uint32_t End;
        bool SignExt;
    };

    struct BinOperand : Operand
    {
        BinOperand(std::string op, OperandPtr lhs, OperandPtr rhs);

        [[nodiscard]] int32_t AsImmediate() const override;

        std::string Op;
        OperandPtr Lhs;
        OperandPtr Rhs;
    };

    OperandPtr Imm(int32_t imm);
    OperandPtr Sym(SymbolBase* sym);
    OperandPtr Reg(Register reg);
    OperandPtr Off(const OperandPtr& offset, const OperandPtr& base);
    OperandPtr Bits(const OperandPtr& imm, uint32_t end, uint32_t beg, bool sign_ext);
    OperandPtr Bin(const std::string& op, const OperandPtr& lhs, const OperandPtr& rhs);
    OperandPtr Add(const OperandPtr& lhs, const OperandPtr& rhs);
    OperandPtr Sub(const OperandPtr& lhs, const OperandPtr& rhs);
}

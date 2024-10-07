#pragma once

#include <vector>
#include <RiscVM/Instruction.hpp>
#include <RiscVM/RiscVM.hpp>

namespace RiscVM
{
    struct Section
    {
        void PushBack(const Instruction&);
        void EmplaceBack(uint32_t offset, RV32IM rv, const std::vector<OperandPtr>& operands);
        void Skip(size_t n);
        [[nodiscard]] uint32_t Size() const;

        uint32_t Offset = -1;
        std::vector<Instruction> Instructions;
        std::vector<char> Data;
    };
}

#pragma once

#include <vector>
#include <RiscVM/Instruction.hpp>
#include <RiscVM/RiscVM.hpp>

namespace RiscVM
{
    struct Section
    {
        void PushBack(const Instruction&);
        void PushBack(int32_t);
        void EmplaceBack(uint32_t offset, uint32_t rv, const std::vector<OperandPtr>& operands);
        void EmplaceBack(uint32_t rv, const std::vector<OperandPtr>& operands);
        void Skip(size_t n);
        [[nodiscard]] uint32_t Size() const;

        uint32_t Offset = -1;
        std::vector<Instruction> Instructions;
        std::vector<char> Data;
    };
}

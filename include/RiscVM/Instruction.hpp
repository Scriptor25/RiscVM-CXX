#pragma once

#include <vector>
#include <RiscVM/ISA.hpp>
#include <RiscVM/RiscVM.hpp>

namespace RiscVM
{
    struct Instruction
    {
        uint32_t Offset;
        uint32_t RV;
        std::vector<OperandPtr> Operands;
    };
}

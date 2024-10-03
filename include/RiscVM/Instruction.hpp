#pragma once

#include <vector>
#include <RiscVM/ISA.hpp>
#include <RiscVM/RiscVM.hpp>

namespace RiscVM
{
    struct Instruction
    {
        uint32_t Offset;
        RV32IM RV;
        std::vector<OperandPtr> Operands;
    };
}

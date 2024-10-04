#pragma once

#include <vector>
#include <RiscVM/Instruction.hpp>
#include <RiscVM/RiscVM.hpp>

namespace RiscVM
{
    struct Section
    {
        void PushBack(const Instruction&);

        uint32_t Offset = -1;
        uint32_t Size = 0;
        std::vector<Instruction> InstructionList;
    };
}

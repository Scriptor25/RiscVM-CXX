#pragma once

#include <memory>

namespace RiscVM
{
    class VM;

    struct Instruction;
    struct Operand;

    typedef std::shared_ptr<Operand> OperandPtr;

    struct Section;
    struct Symbol;
}

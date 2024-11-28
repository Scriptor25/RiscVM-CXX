#pragma once

#include <memory>

namespace RiscVM
{
    class Assembler;
    class VM;

    struct Instruction;
    struct Operand;
    struct Section;
    struct SymbolBase;
    struct SubSymbol;
    struct Symbol;

    typedef std::shared_ptr<Operand> OperandPtr;

    void DumpRaw(const char*, size_t);
    void Dump(const char*, size_t);
}

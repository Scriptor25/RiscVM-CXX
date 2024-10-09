#pragma once

#include <RiscVM/RiscVM.hpp>

namespace RiscVM
{
    struct SymbolBase
    {
        virtual ~SymbolBase() = default;

        Section* Base;
        uint32_t Offset;
    };

    struct SubSymbol : SymbolBase
    {
    };

    struct Symbol : SymbolBase
    {
        bool Global = false;
        std::map<std::string, SubSymbol> SubSymbols;
    };
}

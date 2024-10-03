#pragma once

#include <RiscVM/RiscVM.hpp>

namespace RiscVM
{
    struct Symbol
    {
        Section* Base;
        uint32_t Offset;
    };
}

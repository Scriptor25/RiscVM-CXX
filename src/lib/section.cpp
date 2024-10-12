#include <RiscVM/Instruction.hpp>
#include <RiscVM/Section.hpp>

void RiscVM::Section::PushBack(const Instruction& instruction)
{
    Instructions.push_back(instruction);
    Skip(4);
}

void RiscVM::Section::PushBack(const int32_t i)
{
    Data.push_back(static_cast<char>(i & 0xff));
    Data.push_back(static_cast<char>(i >> 8 & 0xff));
    Data.push_back(static_cast<char>(i >> 16 & 0xff));
    Data.push_back(static_cast<char>(i >> 24 & 0xff));
}

void RiscVM::Section::EmplaceBack(const uint32_t offset, const RV32IM rv, const std::vector<OperandPtr>& operands)
{
    Instructions.emplace_back(offset, rv, operands);
    Skip(4);
}

void RiscVM::Section::EmplaceBack(const RV32IM rv, const std::vector<OperandPtr>& operands)
{
    Instructions.emplace_back(Data.size(), rv, operands);
    Skip(4);
}

void RiscVM::Section::Skip(const size_t n)
{
    Data.resize(Data.size() + n);
}

uint32_t RiscVM::Section::Size() const
{
    return Data.size();
}

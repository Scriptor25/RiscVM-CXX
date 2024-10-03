#include <RiscVM/Instruction.hpp>
#include <RiscVM/Section.hpp>

void RiscVM::Section::PushBack(const Instruction& instruction)
{
    InstructionList.push_back(instruction);
    Size += 4;
}

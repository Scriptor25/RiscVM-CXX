#include <ranges>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/Instruction.hpp>
#include <RiscVM/ISA.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>

std::vector<char> RiscVM::Assembler::Link(LinkInfo& link_info)
{
    size_t off = 0;
    for (auto& [l_name_, l_align_, l_size_, l_offset_] : link_info.Sections)
    {
        auto align = 1 << l_align_;
        if (const auto rem = off % align)
            off += align - rem;

        auto& [offset_, instructions_, data_] = m_Sections[l_name_];
        l_offset_ = offset_ = off;

        if (l_size_) off += l_size_;
        else off += l_size_ = data_.size();
    }

    std::vector<char> dest(off);

    for (auto& [l_name_, l_align_, l_size_, l_offset_] : link_info.Sections)
    {
        auto& [offset_, instructions_, data_] = m_Sections[l_name_];
        if (offset_ < 0) continue;
        memcpy(dest.data() + offset_, data_.data(), data_.size());
        for (auto& [i_offset_, i_rv_, i_operands_] : instructions_)
        {
            const auto ptr = reinterpret_cast<uint32_t*>(dest.data() + offset_ + i_offset_);
            const auto i = static_cast<uint32_t>(i_rv_);
            switch (i_rv_)
            {
            case RV32IM_ADD:
            case RV32IM_SUB:
            case RV32IM_SLL:
            case RV32IM_SLT:
            case RV32IM_SLTU:
            case RV32IM_XOR:
            case RV32IM_SRL:
            case RV32IM_SRA:
            case RV32IM_OR:
            case RV32IM_AND:
            case RV32IM_MUL:
            case RV32IM_MULH:
            case RV32IM_MULHSU:
            case RV32IM_MULHU:
            case RV32IM_DIV:
            case RV32IM_DIVU:
            case RV32IM_REM:
            case RV32IM_REMU:
                {
                    const Format::R x
                    {
                        .Opcode = i & 0b1111111,
                        .Rd = i_operands_[0]->AsRegister(),
                        .Func3 = i >> 7 & 0b111,
                        .Rs1 = i_operands_[1]->AsRegister(),
                        .Rs2 = i_operands_[2]->AsRegister(),
                        .Func7 = i >> 10 & 0b1111111,
                    };
                    *ptr = x.Data;
                }
                break; // R

            case RV32IM_JALR:
                {
                    const auto o = std::dynamic_pointer_cast<OffsetOperand>(i_operands_[1]);
                    Format::I x
                    {
                        .Opcode = i & 0b1111111,
                        .Rd = i_operands_[0]->AsRegister(),
                        .Func3 = i >> 7 & 0b111,
                        .Rs1 = o->Base->AsRegister(),
                    };
                    x.Immediate(o->Offset->AsImmediate());
                    *ptr = x.Data;
                }
                break; // JALR

            case RV32IM_ECALL:
            case RV32IM_EBREAK:
                {
                    const Format::I x
                    {
                        .Opcode = i & 0b1111111,
                        .Func3 = i >> 7 & 0b111,
                    };
                    *ptr = x.Data;
                }
                break; // ENV

            case RV32IM_LB:
            case RV32IM_LH:
            case RV32IM_LW:
            case RV32IM_LBU:
            case RV32IM_LHU:
                {
                    const auto o = std::dynamic_pointer_cast<OffsetOperand>(i_operands_[1]);
                    Format::I x
                    {
                        .Opcode = i & 0b1111111,
                        .Rd = i_operands_[0]->AsRegister(),
                        .Func3 = i >> 7 & 0b111,
                        .Rs1 = o->Base->AsRegister(),
                    };
                    x.Immediate(o->Offset->AsImmediate());
                    *ptr = x.Data;
                }
                break; // LOAD

            case RV32IM_ADDI:
            case RV32IM_SLTI:
            case RV32IM_SLTIU:
            case RV32IM_XORI:
            case RV32IM_ORI:
            case RV32IM_ANDI:
            case RV32IM_SLLI:
            case RV32IM_SRLI:
            case RV32IM_SRAI:
            case RV32IM_FENCE:
                {
                    Format::I x
                    {
                        .Opcode = i & 0b1111111,
                        .Rd = i_operands_[0]->AsRegister(),
                        .Func3 = i >> 7 & 0b111,
                        .Rs1 = i_operands_[1]->AsRegister(),
                    };
                    x.Immediate(i_operands_[2]->AsImmediate());
                    *ptr = x.Data;
                }
                break; // I

            case RV32IM_SB:
            case RV32IM_SH:
            case RV32IM_SW:
                {
                    const auto o = std::dynamic_pointer_cast<OffsetOperand>(i_operands_[1]);
                    Format::S x
                    {
                        .Opcode = i & 0b1111111,
                        .Func3 = i >> 7 & 0b111,
                        .Rs1 = i_operands_[0]->AsRegister(),
                        .Rs2 = o->Base->AsRegister(),
                    };
                    x.Immediate(o->Offset->AsImmediate());
                    *ptr = x.Data;
                }
                break; // S

            case RV32IM_BEQ:
            case RV32IM_BNE:
            case RV32IM_BLT:
            case RV32IM_BGE:
            case RV32IM_BLTU:
            case RV32IM_BGEU:
                {
                    Format::B x
                    {
                        .Opcode = i & 0b1111111,
                        .Func3 = i >> 7 & 0b111,
                        .Rs1 = i_operands_[0]->AsRegister(),
                        .Rs2 = i_operands_[1]->AsRegister(),
                    };
                    x.Immediate(i_operands_[2]->AsImmediate());
                    *ptr = x.Data;
                }
                break; // B

            case RV32IM_LUI:
            case RV32IM_AUIPC:
                {
                    Format::U x
                    {
                        .Opcode = i & 0b1111111,
                        .Rd = i_operands_[0]->AsRegister(),
                    };
                    x.Immediate(i_operands_[1]->AsImmediate() << 12);
                    *ptr = x.Data;
                }
                break; // U

            case RV32IM_JAL:
                {
                    Format::J x
                    {
                        .Opcode = i & 0b1111111,
                        .Rd = i_operands_[0]->AsRegister(),
                    };
                    x.Immediate(i_operands_[1]->AsImmediate());
                    *ptr = x.Data;
                }
                break; // J
            }
        }
    }

    return std::move(dest);
}

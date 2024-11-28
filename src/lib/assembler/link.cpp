#include <cstring>
#include <ranges>
#include <RiscVM/Assembler.hpp>
#include <RiscVM/Instruction.hpp>
#include <RiscVM/ISA.hpp>
#include <RiscVM/Operand.hpp>
#include <RiscVM/Section.hpp>

void RiscVM::Assembler::Link(LinkInfo& link_info, std::vector<char>& dest)
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

    dest.resize(off);

    for (auto& [l_name_, l_align_, l_size_, l_offset_] : link_info.Sections)
    {
        auto& [offset_, instructions_, data_] = m_Sections[l_name_];
        if (offset_ == static_cast<uint32_t>(-1)) continue;
        memcpy(dest.data() + offset_, data_.data(), data_.size());
        for (auto& [i_offset_, i_rv_, i_operands_] : instructions_)
        {
            const auto ptr = reinterpret_cast<uint32_t*>(dest.data() + offset_ + i_offset_);
            const auto i = i_rv_;
            switch (i_rv_)
            {
            case RV32I_ADD:
            case RV32I_SUB:
            case RV32I_SLL:
            case RV32I_SLT:
            case RV32I_SLTU:
            case RV32I_XOR:
            case RV32I_SRL:
            case RV32I_SRA:
            case RV32I_OR:
            case RV32I_AND:
            case RV32M_MUL:
            case RV32M_MULH:
            case RV32M_MULHSU:
            case RV32M_MULHU:
            case RV32M_DIV:
            case RV32M_DIVU:
            case RV32M_REM:
            case RV32M_REMU:
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

            case RV32I_JALR:
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

            case RV32I_ECALL:
            case RV32I_EBREAK:
                {
                    const Format::I x
                    {
                        .Opcode = i & 0b1111111,
                        .Func3 = i >> 7 & 0b111,
                    };
                    *ptr = x.Data;
                }
                break; // ENV

            case RV32I_LB:
            case RV32I_LH:
            case RV32I_LW:
            case RV32I_LBU:
            case RV32I_LHU:
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

            case RV32I_ADDI:
            case RV32I_SLTI:
            case RV32I_SLTIU:
            case RV32I_XORI:
            case RV32I_ORI:
            case RV32I_ANDI:
            case RV32I_SLLI:
            case RV32I_SRLI:
            case RV32I_SRAI:
            case RV32I_FENCE:
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

            case RV32I_SB:
            case RV32I_SH:
            case RV32I_SW:
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

            case RV32I_BEQ:
            case RV32I_BNE:
            case RV32I_BLT:
            case RV32I_BGE:
            case RV32I_BLTU:
            case RV32I_BGEU:
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

            case RV32I_LUI:
            case RV32I_AUIPC:
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

            case RV32I_JAL:
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
            default: break;
            }
        }
    }
}

#pragma once

#include <cstdint>
#include <functional>
#include <map>

namespace RiscVM
{
    typedef std::function<void(class VM& vm)> ECall;

    class VM
    {
    public:
        void Reset();
        void Load(const char* pgm, size_t len);
        bool Cycle();

        [[nodiscard]] char* Memory() const;
        [[nodiscard]] size_t MemorySize() const;

        bool& Ok();
        int32_t& Status();

        int32_t& R(uint32_t);

        std::map<int, ECall>& ECallMap();

    private:
        void Exec(uint32_t data);

        void LUI(uint32_t rd, int32_t imm);
        void AUIPC(uint32_t rd, int32_t imm);
        void JAL(uint32_t rd, int32_t imm);
        void JALR(uint32_t rd, uint32_t rs1, int32_t imm);
        void BEQ(uint32_t rs1, uint32_t rs2, int32_t imm);
        void BNE(uint32_t rs1, uint32_t rs2, int32_t imm);
        void BLT(uint32_t rs1, uint32_t rs2, int32_t imm);
        void BGE(uint32_t rs1, uint32_t rs2, int32_t imm);
        void BLTU(uint32_t rs1, uint32_t rs2, int32_t imm);
        void BGEU(uint32_t rs1, uint32_t rs2, int32_t imm);
        void LB(uint32_t rd, uint32_t rs1, int32_t imm);
        void LH(uint32_t rd, uint32_t rs1, int32_t imm);
        void LW(uint32_t rd, uint32_t rs1, int32_t imm);
        void LBU(uint32_t rd, uint32_t rs1, int32_t imm);
        void LHU(uint32_t rd, uint32_t rs1, int32_t imm);
        void SB(uint32_t rs1, uint32_t rs2, int32_t imm);
        void SH(uint32_t rs1, uint32_t rs2, int32_t imm);
        void SW(uint32_t rs1, uint32_t rs2, int32_t imm);
        void ADDI(uint32_t rd, uint32_t rs1, int32_t imm);
        void SLTI(uint32_t rd, uint32_t rs1, int32_t imm);
        void SLTIU(uint32_t rd, uint32_t rs1, int32_t imm);
        void XORI(uint32_t rd, uint32_t rs1, int32_t imm);
        void ORI(uint32_t rd, uint32_t rs1, int32_t imm);
        void ANDI(uint32_t rd, uint32_t rs1, int32_t imm);
        void SLLI(uint32_t rd, uint32_t rs1, uint32_t imm);
        void SRLI(uint32_t rd, uint32_t rs1, uint32_t imm);
        void SRAI(uint32_t rd, uint32_t rs1, uint32_t imm);
        void ADD(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void SUB(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void SLL(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void SLT(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void SLTU(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void XOR(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void SRL(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void SRA(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void OR(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void AND(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void FENCE(uint32_t rd, uint32_t rs1, uint32_t fm_pred_succ);
        void ECALL();
        void EBREAK();

        void MUL(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void MULH(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void MULHSU(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void MULHU(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void DIV(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void DIVU(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void REM(uint32_t rd, uint32_t rs1, uint32_t rs2);
        void REMU(uint32_t rd, uint32_t rs1, uint32_t rs2);

    private:
        int32_t m_Registers[32]{};
        int32_t m_PC = 0;
        int32_t m_Status = 0;

        char* m_Memory = nullptr;
        size_t m_MemorySize = 0;

        bool m_DirtyPC = false;
        bool m_Ok = true;

        std::map<int, ECall> m_ECallMap;
    };
}

#ifndef LISC16_HPP
#define LISC16_HPP

#include <cstdint>

namespace lisc
{
  enum opcodes
  {
    // Jumps and Conditions
    JIE = 0x0, // Jump if equal
    JAL = 0x1, // Jump and link PC+1

    // Load Store Set
    STR = 0x4, // Store in memory
    LOD = 0x5, // Load from memory
    SHB = 0x6, // Set high 8 bits
    SLB = 0x7, // Set low 8 bits

    // Bitwise Operators
    AND = 0x8, // And
    NND = 0x9, // Not And
    IOR = 0xA, // Inclusive Or
    XOR = 0xB, // Exclusive Or

    // Math Operators
    ADD = 0xC, // Add
    SUB = 0xD, // Subtract
    MUL = 0xE, // Multiply
    DIV = 0xF  // Divide
  };

  struct Instruction
  {
    constexpr Instruction() :
      opcode{0}, rega{0}, byte{0} {}

    constexpr Instruction(
      const opcodes& op, const uint8_t& ra,
      const uint8_t& rb, const uint8_t& rc) :
      opcode{op}, rega{ra}, regb{rb}, regc{rc} {}

    constexpr Instruction(
      const opcodes& op, const uint8_t& ra,
      const uint8_t b) :
      opcode{op}, rega{ra}, byte{b} {}

    unsigned opcode : 4;
    unsigned rega : 4;
    union
    {
      struct { unsigned regb : 4, regc : 4; };
      unsigned byte : 8;
    };
  };

  class CPU
  {
  public:

  public: // Instructions
    void runInstruction(const Instruction& data)
    {
      if(data.opcode == opcodes::JIE)
      { JIE(data); }
      else if(data.opcode == opcodes::JAL)
      { JAL(data); }

      else if(data.opcode == opcodes::STR)
      { STR(data); }
      else if(data.opcode == opcodes::LOD)
      { LOD(data); }
      else if(data.opcode == opcodes::SHB)
      { SHB(data); }
      else if(data.opcode == opcodes::SLB)
      { SLB(data); }

      else if(data.opcode == opcodes::AND)
      { AND(data); }
      else if(data.opcode == opcodes::NND)
      { NND(data); }
      else if(data.opcode == opcodes::IOR)
      { IOR(data); }
      else if(data.opcode == opcodes::XOR)
      { XOR(data); }

      else if(data.opcode == opcodes::ADD)
      { LOD(data); }
      else if(data.opcode == opcodes::SUB)
      { LOD(data); }
      else if(data.opcode == opcodes::MUL)
      { MUL(data); }
      else if(data.opcode == opcodes::DIV)
      { DIV(data); }

      else { NO_OPCODE(data); }
    }

    /* Jumps/Conditions */
    void JIE(const Instruction& data)
    {
      if(reg[data.rega] == reg[data.regb])
      { PC = data.regc; }
    }

    void JAL(const Instruction& data)
    {
      reg[data.rega] = PC+1;
      PC = data.regb;
    }

    /* Load/Store/Set */
    void STR(const Instruction& data)
    {
      mem[(data.regb << 16) | reg[data.regc]] = reg[data.rega];
    }

    void LOD(const Instruction& data)
    {
      reg[data.rega] = mem[(data.regb << 16) | reg[data.regc]];
    }

    void SHB(const Instruction& data)
    {
      reg[data.rega] &= 0xff00;
      reg[data.rega] |= data.byte << 8;
    }

    void SLB(const Instruction& data)
    {
      reg[data.rega] &= 0x00ff;
      reg[data.rega] |= data.byte;
    }

    /* Bitwise Operators */
    void AND(const Instruction& data)
    {
      reg[data.rega] = reg[data.regb] & reg[data.regc];
    }

    void NND(const Instruction& data)
    {
      reg[data.rega] = 0xffff ^ (reg[data.regb] & reg[data.regc]);
    }

    void IOR(const Instruction& data)
    {
      reg[data.rega] = reg[data.regb] | reg[data.regc];
    }

    void XOR(const Instruction& data)
    {
      reg[data.rega] = reg[data.regb] ^ reg[data.regc];
    }

    /* Mathmatical Operators */
    void ADD(const Instruction& data)
    {
      reg[data.rega] = reg[data.regb] + reg[data.regc];
    }

    void SUB(const Instruction& data)
    {
      reg[data.rega] = reg[data.regb] - reg[data.regc];
    }

    void MUL(const Instruction& data)
    {
      reg[data.rega] = reg[data.regb] * reg[data.regc];
    }

    void DIV(const Instruction& data)
    {
      reg[data.rega] = reg[data.regb] / reg[data.regc];
    }

    void NO_OPCODE(const Instruction& info){}
  private:
    uint16_t PC = 0;
    Instruction program[0x10000] = { };

    uint16_t reg[0x10] = { };
    uint16_t mem[0x100000] = { };
  };
}

#endif

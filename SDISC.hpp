#ifndef SDISC_HPP
#define SDISC_HPP

#include <cstdint>
#include <vector>
#include <array>

namespace SDISC // Define Types
{
  using BYTE = uint8_t;
  using WORD = uint16_t;
  using COUNT = uint64_t;
}

namespace SDISC // OP-Codes
{
  namespace OP
  {
    // Program Control
    const BYTE NOP = 0x0; // No Operation
    const BYTE STP = 0x1; // Stop

    // Jumps and Conditions
    const BYTE JIE = 0x2; // Jump if equal
    const BYTE JAL = 0x3; // Jump and link PC+1

    // Load Store Set
    const BYTE STR = 0x4; // Store in memory
    const BYTE LOD = 0x5; // Load from memory
    const BYTE SHB = 0x6; // Set high 8 bits
    const BYTE SLB = 0x7; // Set low 8 bits

    // Bitwise Operators
    const BYTE AND = 0x8; // And
    const BYTE NND = 0x9; // Not And
    const BYTE IOR = 0xA; // Inclusive Or
    const BYTE XOR = 0xB; // Exclusive Or

    // Math Operators
    const BYTE ADD = 0xC; // Add
    const BYTE SUB = 0xD; // Subtract
    const BYTE DIV = 0xE; // Multiply
    const BYTE MUL = 0xF; // Divide

    // Ticks
    const COUNT tick_count[0x10] =
    {
      // Program Control
      2, 2,
      // Jumps and Conditions
      6, 8,
      // Load Store Set
      12, 8, 4, 4,
      // Bitwise Operators
      4, 4, 4, 4,
      // Math Operators
      8, 8, 16, 32
    };
  }
}

namespace SDISC // Instructions
{
  struct Instruction
  {
  public: // Constructor
    Instruction() : code{OP::STP}, rega{0}, byte{0} {}

    Instruction(const Instruction& in)
      : code{in.code}, rega{in.rega}, byte{in.byte} {}

    Instruction(const BYTE& op) : code{op}, rega{0}, byte{0} {}

    Instruction(const BYTE& op, const BYTE& ra,
                const BYTE& rb, const BYTE& rc)
                : code{op}, rega{ra}
                , regb{rb}, regc{rc} {}

    Instruction(const BYTE& op, const BYTE& ra, const BYTE& b)
                : code{op}, rega{ra}, byte{b} {}

    BYTE code : 4, rega : 4;
    union
    {
      struct { BYTE regb : 4, regc : 4; };
      BYTE byte : 8;
    };
  };
}

namespace SDISC // Constants
{
  const std::size_t mem_size = 0x10000;
  const std::size_t pro_size = 0x10000;
  const std::size_t reg_size = 0x10;

  const WORD init_mem = 0xffff;
  const WORD init_reg = 0x0000;
  const Instruction init_pro = Instruction();
}

namespace SDISC
{
  enum STATE { running, paused, stopped };

  class CPU
  {
  public: // CPU Control
    CPU(){ reset(); }

    void reset()
    {
      tick = 0;
      status = STATE::stopped;

      for(Instruction& i : program) i = init_pro;
      for(WORD& i : mem) i = init_mem;
      for(WORD& i : reg) i = init_reg;
    }

    template<std::size_t size>
    void loadProgram(const std::array<Instruction, size>& in_program)
    {
      for(std::size_t i = 0; i < pro_size; i++)
      {
        if(i < size) program[i] = in_program[i];
        else program[i] = init_pro;
      }
    }

    void loadProgram(const std::vector<Instruction>& in_program)
    {
      for(std::size_t i = 0; i < pro_size; i++)
      {
        if(i < in_program.size()) program[i] = in_program[i];
        else program[i] = init_pro;
      }
    }

    void start(const WORD start_pc = 0)
    {
      PC = start_pc;
      status = STATE::running;

      while(status != STATE::stopped)
      {
        if(status == STATE::running)
          RUN(program[PC++]);
      }
    }

  public: // Instructions
    /* Execute Instruction */
    COUNT RUN(const Instruction&);

    /* Program Control */
    COUNT NOP(const Instruction&); // 2 Ticks
    COUNT STP(const Instruction&); // 2 Ticks

    /* Jumps/Conditions */
    COUNT JIE(const Instruction&); // 6 Ticks
    COUNT JAL(const Instruction&); // 8 Ticks

    /* Load/Store/Set */
    COUNT STR(const Instruction&); // 12 Ticks
    COUNT LOD(const Instruction&); // 8 Ticks
    COUNT SHB(const Instruction&); // 4 Ticks
    COUNT SLB(const Instruction&); // 4 Ticks

    /* Bitwise Operators */
    COUNT AND(const Instruction&); // 4 Ticks
    COUNT NND(const Instruction&); // 4 Ticks
    COUNT IOR(const Instruction&); // 4 Ticks
    COUNT XOR(const Instruction&); // 4 Ticks

    /* Mathmatical Operators */
    COUNT ADD(const Instruction&); // 8 Ticks
    COUNT SUB(const Instruction&); // 8 Ticks
    COUNT MUL(const Instruction&); // 16 Ticks
    COUNT DIV(const Instruction&); // 32 Ticks

    /* Clock Function */
    COUNT addTicks(const Instruction& data)
    {
      tick += OP::tick_count[data.code];
      return  OP::tick_count[data.code];
    }

  public: // Variables
    COUNT tick = 0;
    STATE status = STATE::stopped;

    WORD PC = 0;
    std::array<Instruction, pro_size> program = { };
    std::array<WORD, reg_size> reg = { };
    std::array<WORD, mem_size> mem = { };
  };
}

namespace SDISC
{
  /* Execute Instruction */
  COUNT CPU::RUN(const Instruction& data)
  {
    // Program Control
    if(data.code == OP::NOP)
    { return NOP(data); }
    else if(data.code == OP::STP)
    { return STP(data); }

    // Jump/Condition
    else if(data.code == OP::JIE)
    { return JIE(data); }
    else if(data.code == OP::JAL)
    { return JAL(data); }

    // Store/Load/Set
    else if(data.code == OP::STR)
    { return STR(data); }
    else if(data.code == OP::LOD)
    { return LOD(data); }
    else if(data.code == OP::SHB)
    { return SHB(data); }
    else if(data.code == OP::SLB)
    { return SLB(data); }

    // Bitwise
    else if(data.code == OP::AND)
    { return AND(data); }
    else if(data.code == OP::NND)
    { return NND(data); }
    else if(data.code == OP::IOR)
    { return IOR(data); }
    else if(data.code == OP::XOR)
    { return XOR(data); }

    // Math
    else if(data.code == OP::ADD)
    { return ADD(data); }
    else if(data.code == OP::SUB)
    { return SUB(data); }
    else if(data.code == OP::MUL)
    { return MUL(data); }
    else if(data.code == OP::DIV)
    { return DIV(data); }

    return 0;
  }

  /* Program Control */
  COUNT CPU::NOP(const Instruction& data)
  {
    return addTicks(data);
  }

  COUNT CPU::STP(const Instruction& data)
  {
    status = STATE::stopped;

    return addTicks(data);
  }

  /* Jumps/Conditions */
  COUNT CPU::JIE(const Instruction& data)
  {
    if(reg[data.rega] == reg[data.regb])
    { PC = reg[data.regc]; }

    return addTicks(data);
  }

  COUNT CPU::JAL(const Instruction& data)
  {
    WORD start_pc = PC+1;
    PC = reg[data.regb];
    reg[data.rega] = start_pc;

    return addTicks(data);
  }

  /* Load/Store/Set */
  COUNT CPU::STR(const Instruction& data)
  {
    mem[reg[data.regb]] = reg[data.rega];

    return addTicks(data);
  }

  COUNT CPU::LOD(const Instruction& data)
  {
    reg[data.rega] = mem[reg[data.regb]];

    return addTicks(data);
  }

  COUNT CPU::SHB(const Instruction& data)
  {
    reg[data.rega] &= 0x00ff;
    reg[data.rega] |= data.byte << 8;

    return addTicks(data);
  }

  COUNT CPU::SLB(const Instruction& data)
  {
    reg[data.rega] &= 0xff00;
    reg[data.rega] |= data.byte;

    return addTicks(data);
  }

  /* Bitwise Operators */
  COUNT CPU::AND(const Instruction& data)
  {
    reg[data.rega] = reg[data.regb] & reg[data.regc];

    return addTicks(data);
  }

  COUNT CPU::NND(const Instruction& data)
  {
    reg[data.rega] = ~(reg[data.regb] & reg[data.regc]);

    return addTicks(data);
  }

  COUNT CPU::IOR(const Instruction& data)
  {
    reg[data.rega] = reg[data.regb] | reg[data.regc];

    return addTicks(data);
  }

  COUNT CPU::XOR(const Instruction& data)
  {
    reg[data.rega] = reg[data.regb] ^ reg[data.regc];

    return addTicks(data);
  }

  /* Mathmatical Operators */
  COUNT CPU::ADD(const Instruction& data)
  {
    reg[data.rega] = reg[data.regb] + reg[data.regc];

    return addTicks(data);
  }

  COUNT CPU::SUB(const Instruction& data)
  {
    reg[data.rega] = reg[data.regb] - reg[data.regc];

    return addTicks(data);
  }

  COUNT CPU::MUL(const Instruction& data)
  {
    reg[data.rega] = reg[data.regb] * reg[data.regc];

    return addTicks(data);
  }

  COUNT CPU::DIV(const Instruction& data)
  {
    reg[data.rega] = reg[data.regb] / reg[data.regc];

    return addTicks(data);
  }
}

#endif

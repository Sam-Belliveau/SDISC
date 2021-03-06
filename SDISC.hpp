#ifndef SDISC_HPP
#define SDISC_HPP

#include <cstdint>
#include <algorithm>

namespace SDISC // Define Types
{
  using BYTE = std::uint8_t;
  using WORD = std::uint16_t;
  using COUNT = std::uint64_t;
}

namespace SDISC // OP-Codes
{
  namespace OP
  {
    enum
    {
      // Program
      STP = 0x0, // Stop

      // Jumps and Conditions
      JAL = 0x1, // Jump and link
      JIE = 0x2, // Jump if equal
      JIL = 0x3, // Jump if less than

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
      DIV = 0xE, // Multiply
      MUL = 0xF  // Divide
    };

    // Ticks
    const COUNT tick_count[0x10] =
    {
      // Program Control
      0,
      // Jumps and Conditions
      4, 6, 6,
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
    Instruction()
      : data{0} {}

    Instruction(const Instruction& in)
      : data{in.data} {}

    Instruction(const BYTE& op)
    {
      data =  0x0000;
      data |= (op & 0xf) << 12;
    }

    Instruction(const BYTE& op, const BYTE& ra,
                const BYTE& rb, const BYTE& rc)
    {
        data =  0x0000;
        data |= (op & 0xf) << 12;
        data |= (ra & 0xf) << 8;
        data |= (rb & 0xf) << 4;
        data |= (rc & 0xf) << 0;
    }

    Instruction(const BYTE& op, const BYTE& ra, const BYTE& b)
    {
      data =  0x0000;
      data |= (op & 0xf) << 12;
      data |= (ra & 0xf) << 8;
      data |= (b & 0xff) << 0;
    }

    BYTE code() const { return (data >> 12) & 0xf; }
    BYTE rega() const { return (data >> 8)  & 0xf; }
    BYTE regb() const { return (data >> 4)  & 0xf; }
    BYTE regc() const { return (data >> 0)  & 0xf; }
    BYTE byte() const { return (data >> 0)  & 0xff; }

  private:
    WORD data;
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
  class CPU
  {
  public: // CPU Control
    CPU(){ reset(); }

    void reset()
    {
      tick = 0;
      for(Instruction& i : program) i = init_pro;
      for(WORD& i : mem) i = init_mem;
      for(WORD& i : reg) i = init_reg;
    }

    template<class ArrayType>
    void loadProgram(const ArrayType in_program)
    {
      for(Instruction& a : program) a = Instruction();
      std::copy(in_program.begin(), in_program.end(), program);
    }

  public: // Instructions
    /* Execute Instruction */
    COUNT CYCLE() { return RUN(program[PC++]); }
    COUNT RUN(const Instruction&);

    /* Program Control */
    COUNT STP(const Instruction&); // 0 Ticks

    /* Jumps/Conditions */
    COUNT JAL(const Instruction&); // 4 Ticks
    COUNT JIE(const Instruction&); // 6 Ticks
    COUNT JIL(const Instruction&); // 6 Ticks

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
      tick += OP::tick_count[data.code()];
      return  OP::tick_count[data.code()];
    }

  public: // Variables
    WORD PC = 0;
    Instruction program[pro_size];
    WORD reg[reg_size];
    WORD mem[mem_size];

    COUNT tick = 0;
  };
}

namespace SDISC
{
  /* Execute Instruction */
  COUNT CPU::RUN(const Instruction& data)
  {
    // Program Control
    if(data.code() == OP::STP)
    { return STP(data); }

    // Jump/Condition
    else if(data.code() == OP::JAL)
    { return JAL(data); }
    else if(data.code() == OP::JIE)
    { return JIE(data); }
    else if(data.code() == OP::JIL)
    { return JIL(data); }

    // Store/Load/Set
    else if(data.code() == OP::STR)
    { return STR(data); }
    else if(data.code() == OP::LOD)
    { return LOD(data); }
    else if(data.code() == OP::SHB)
    { return SHB(data); }
    else if(data.code() == OP::SLB)
    { return SLB(data); }

    // Bitwise
    else if(data.code() == OP::AND)
    { return AND(data); }
    else if(data.code() == OP::NND)
    { return NND(data); }
    else if(data.code() == OP::IOR)
    { return IOR(data); }
    else if(data.code() == OP::XOR)
    { return XOR(data); }

    // Math
    else if(data.code() == OP::ADD)
    { return ADD(data); }
    else if(data.code() == OP::SUB)
    { return SUB(data); }
    else if(data.code() == OP::MUL)
    { return MUL(data); }
    else if(data.code() == OP::DIV)
    { return DIV(data); }

    return 0;
  }

  /* Program Control */
  // Stops Program [No Inputs]
  COUNT CPU::STP(const Instruction& data)
  { --PC; return addTicks(data); }

  /* Jumps/Conditions */
  // Stores current PC in rega then jumps to address in regb
  COUNT CPU::JAL(const Instruction& data)
  {
    const WORD old_PC = ++PC;
    PC = reg[data.regb()];
    reg[data.rega()] = old_PC;

    return addTicks(data);
  }

  // If rega and regb are equal, jump to address in regc
  COUNT CPU::JIE(const Instruction& data)
  {
    if(reg[data.rega()] == reg[data.regb()])
    { PC = reg[data.regc()]; }

    return addTicks(data);
  }

  // If rega is less than regb, jump to address in regc
  COUNT CPU::JIL(const Instruction& data)
  {
    if(reg[data.rega()] < reg[data.regb()])
    { PC = reg[data.regc()]; }

    return addTicks(data);
  }

  /* Load/Store/Set */
  // Set mem address in regb to rega
  COUNT CPU::STR(const Instruction& data)
  {
    mem[reg[data.regb()]] = reg[data.rega()];

    return addTicks(data);
  }

  // Set rega to mem address in regb
  COUNT CPU::LOD(const Instruction& data)
  {
    reg[data.rega()] = mem[reg[data.regb()]];

    return addTicks(data);
  }

  // Set MS-8 bits to byte
  COUNT CPU::SHB(const Instruction& data)
  {
    reg[data.rega()] &= 0x00ff;
    reg[data.rega()] |= data.byte() << 8;

    return addTicks(data);
  }

  // Set LS-8 bits to byte
  COUNT CPU::SLB(const Instruction& data)
  {
    reg[data.rega()] &= 0xff00;
    reg[data.rega()] |= data.byte();

    return addTicks(data);
  }

  /* Bitwise Operators */
  // And regb and regc and store it in rega
  COUNT CPU::AND(const Instruction& data)
  {
    reg[data.rega()] = reg[data.regb()] & reg[data.regc()];

    return addTicks(data);
  }

  // Not And regb and regc and store it in rega
  COUNT CPU::NND(const Instruction& data)
  {
    reg[data.rega()] = ~(reg[data.regb()] & reg[data.regc()]);

    return addTicks(data);
  }

  // Inclusive Or regb and regc and store it in rega
  COUNT CPU::IOR(const Instruction& data)
  {
    reg[data.rega()] = reg[data.regb()] | reg[data.regc()];

    return addTicks(data);
  }

  // Exclusive Or regb and regc and store it in rega
  COUNT CPU::XOR(const Instruction& data)
  {
    reg[data.rega()] = reg[data.regb()] ^ reg[data.regc()];

    return addTicks(data);
  }

  /* Mathmatical Operators */
  // Add regb and regc and store it in rega
  COUNT CPU::ADD(const Instruction& data)
  {
    reg[data.rega()] = reg[data.regb()] + reg[data.regc()];

    return addTicks(data);
  }

  // Subtract regb by regc and store it in rega
  COUNT CPU::SUB(const Instruction& data)
  {
    reg[data.rega()] = reg[data.regb()] - reg[data.regc()];

    return addTicks(data);
  }

  // Multiply regb and regc and store it in rega
  COUNT CPU::MUL(const Instruction& data)
  {
    reg[data.rega()] = reg[data.regb()] * reg[data.regc()];

    return addTicks(data);
  }

  // Divide regb by regc and store it in rega
  COUNT CPU::DIV(const Instruction& data)
  {
    reg[data.rega()] = reg[data.regb()] / reg[data.regc()];

    return addTicks(data);
  }
}

#endif

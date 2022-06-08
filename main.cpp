#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <bitset>
#include <cstdint>
using namespace std;

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

enum class Opcode
{
  ADD = 0b000,
  NAND = 0b001,
  LW = 0b010,
  SW = 0b011,
  BEQ = 0b100,
  JALR = 0b101,
  HALT = 0b110,
  NOOP = 0b111
};


struct stateStruct 
{
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
};


class Simulator
{
  public:
  stateStruct s;
  uint32_t instruction_bits;
  uint32_t execution_counter = 0;
  string file_name;


  Simulator(string file_name)
  {
    this->file_name = file_name;
  }

  void simulator_start()
  {
    ifstream file(file_name, ios_base::in);
    s.pc = 0;
    s.numMemory = 0;

    for(int i = 0; i < NUMREGS; i++)
    {
      s.reg[i] = 0;
    }

    //Checks if file is valid; otherwise exit
    if(!file.is_open()) 
    {
      cout << "error in opening input file\n";
      exit(1);
    }

    //Loads instructions into memory
    for(int i = 0;file >> instruction_bits; i++)
    {
      s.mem[i] = instruction_bits;
      s.numMemory++;
    }

    //Takes in instructions; one by one. Applies the functional operation then increments PC.
    //for(int i = 0,run=1; i < 50&&run; i++)
    for(int run = 1; run == 1;)
    {
      printState(&s);

      switch (opcode(s.mem[s.pc])) 
      {
        case Opcode::ADD:
        //cout<<"add"<<endl;
        ADD_operation();
        break;
        case Opcode::NAND:
        //cout<<"nand"<<endl;
        NAND_operation();
        break;
        case Opcode::LW:
        //cout<<"lw"<<endl;
        LW_operation();
        break;
        case Opcode::SW:
        //cout<<"sw"<<endl;
        SW_operation();
        break;
        case Opcode::BEQ:
        //cout<<"beq"<<endl;
        BEQ_operation();
        break;
        case Opcode::JALR:
        //cout<<"jalr"<<endl;
        JALR_operation();
        break;
        case Opcode::HALT:
        run = HALT_operation();
        break;
        case Opcode::NOOP:
        //cout<<"noop"<<endl;
        NOOP_operation();
        break;
      }

      (s.pc)++;
      execution_counter++;
    }

    cout<<"Machine Halted"<<endl;
    cout<<"total of "<<execution_counter<<" instructions executed\n";
    cout<<"final state of machine:\n";
    printState(&s);
  }

  void ADD_operation()
  {
    s.reg[dest_reg(s.mem[s.pc])] = 
    s.reg[reg_a(s.mem[s.pc])] + 
    s.reg[reg_b(s.mem[s.pc])];
  }

  void NAND_operation() 
  {
    int a = s.reg[reg_a(s.mem[s.pc])];
    int b = s.reg[reg_b(s.mem[s.pc])];
    int nand_result = 0;

    int top_bit = -1;
    for(int i=31; i>=0; --i) 
    {
      if(((a>>i)&1) || ((b>>i)&1)) 
      {
          top_bit = i;
          break;
      }
    }

    if(top_bit == -1) 
    {
        nand_result = 1;
    } 

    else 
    {
        for(int i=0; i<=top_bit; ++i) {
            if(!((a>>i)&1) || !((b>>i)&1)) {
                nand_result |= 1 << i;
            }
        }
    }
    
    s.reg[dest_reg(s.mem[s.pc])] = nand_result;

    // s.reg[dest_reg(s.mem[s.pc])] = 
    // ~(s.reg[reg_a(s.mem[s.pc])] & 
    // s.reg[reg_b(s.mem[s.pc])]);
  }

  void LW_operation()
  {
    s.reg[reg_b(s.mem[s.pc])] =
    s.mem[s.reg[reg_a(s.mem[s.pc])] + 
    signed_offset(s.mem[s.pc])];
  }

  void SW_operation()
  {
    s.mem[s.reg[(reg_a(s.mem[s.pc]))] + 
    signed_offset(s.mem[s.pc])] =
    s.reg[reg_b(s.mem[s.pc])];

    // s.mem[s.pc] =
    // s.mem[s.reg[(reg_a(s.mem[s.pc]))] + 
    // signed_offset(s.mem[s.pc])];
  }

  void BEQ_operation()
  {
    if(s.reg[reg_a(s.mem[s.pc])] == 
    s.reg[reg_b(s.mem[s.pc])])
    {
      s.pc = s.pc + signed_offset(s.mem[s.pc]);
    }
  }

  void JALR_operation()
  { 
    // cout<<"s.reg[reg_a(s.mem[s.pc])]: "<< s.reg[reg_a(s.mem[s.pc])]<<endl;
    // cout<<"reg 5: "<< s.reg[6];
    s.reg[reg_b(s.mem[s.pc])] = s.pc + 1;
    s.pc = s.reg[reg_a(s.mem[s.pc])] - 1;
    // s.pc = s.pc + reg_a(s.mem[s.pc]);
  }

  int HALT_operation()
  {
    return 0;
  }

  void NOOP_operation()
  {
    //do nothing
  }

  Opcode opcode(int num)
  {
    return static_cast<Opcode>((num >> 22) & 0b111);
  }

  uint reg_a(int num)
  {
    return (num >> 19) & 0b111;
  }

  uint reg_b(int num)
  {
    return (num >> 16) & 0b111;
  }

  uint dest_reg(int num)
  {
    return (num) & 0b111;
  }

  uint16_t offset_field(int num)
  {
    return (num & 0xffff);
  }

  int16_t signed_offset(int num)
  {
    return (num & 0xffff);
  }

  void printState(stateStruct *statePtr)
  {
      int i;
      printf("\n@@@\nstate:\n");
      printf("\tpc %d\n", statePtr->pc);
      printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
      printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
      printf("end state\n");
  }
};

int main(int argc, char *argv[])
{
  if (argc != 2) 
  {
    printf("error: usage: %s <machine-code-file>\n",
        argv[0]);
    exit(1);
  }

  Simulator simulator(argv[1]);
  //Simulator simulator("test");
  simulator.simulator_start();
  
}
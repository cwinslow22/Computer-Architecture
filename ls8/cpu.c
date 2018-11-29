#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6

/**
 * Add RAM functions read and write
 */
 unsigned char cpu_ram_read(struct cpu *cpu, unsigned char MAR) 
 {
   return cpu->ram[MAR];
 }

 void cpu_ram_write(struct cpu *cpu, unsigned char MAR, unsigned char MDR)
 {
   cpu->ram[MAR] = MDR;
 }

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filename)
{
  // char data[DATA_LEN] = {
  //   // From print8.ls8
  //   0b10000010, // LDI R0,8
  //   0b00000000,
  //   0b00001000,
  //   0b01000111, // PRN R0
  //   0b00000000,
  //   0b00000001  // HLT
  // };

  int address = 0;

  // for (int i = 0; i < DATA_LEN; i++) {
  //   cpu->ram[address++] = data[i];
  // }

  // TODO: Replace this with something less hard-coded
  FILE *fp = fopen(filename, "r");
  char line[8192];

  if (fp == NULL) {
    printf("Error opening file\n");
    exit(1);
  }

  while (fgets(line, sizeof line, fp) != NULL) {
    // char *endchar;
    cpu->ram[address++] = strtoul(line, NULL, 2);
    if (line == NULL) {
      continue;
    }
  }
  fclose(fp);
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      // TODO
      cpu->reg[regA] = cpu->reg[regB] * cpu->reg[regA];
      break;

    // TODO: implement more ALU ops
    case ALU_ADD:
      cpu->reg[regA] = cpu->reg[regB] + cpu->reg[regA];
      break;
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char IR = cpu_ram_read(cpu, cpu->PC);
    unsigned char operandA = cpu_ram_read(cpu, cpu->PC+1);
    unsigned char operandB = cpu_ram_read(cpu, cpu->PC+2);

    unsigned char advance_pc = (IR >> 6) + 1;
    // 2. switch() over it to decide on a course of action.
    switch(IR) {

      case HLT:
        running = 0;
        break;

      case LDI:
        cpu->reg[operandA] = operandB;
        break;

      case PRN:
        printf("%d\n", cpu->reg[operandA]);
        break;

      case MUL:
        alu(cpu, ALU_MUL, operandA, operandB);
        break;

      case ADD:
        alu(cpu, ALU_ADD, operandA, operandB);
        break;

      case PUSH:
        cpu->reg[SP]--;
        cpu_ram_write(cpu, cpu->reg[SP], cpu->reg[operandA]);
        break;

      case POP:
        cpu->reg[operandA] = cpu_ram_read(cpu, cpu->reg[SP]);
        cpu->reg[SP]++;
        break;

      case CALL:
        cpu->reg[SP]--;
        cpu_ram_write(cpu, cpu->reg[SP], cpu->PC + advance_pc);
        cpu->PC = cpu->reg[operandA];
        cpu->FL = 1;
        break;

      case RET:
        cpu->PC = cpu_ram_read(cpu, cpu->reg[SP]);
        cpu->reg[SP]++;
        cpu->FL = 1;
        break;

      case NOP:
        break;

      default:
        printf("Default case triggered\n");
        break;

    }
    if (cpu->FL == 0) {
      cpu->PC += advance_pc;
    }
    else {
      cpu->FL = 0;
      continue;
    }
    
    // 3. Do whatever the instruction should do according to the spec.
    // 4. Move the PC to the next instruction.
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->PC = 0;
  cpu->FL = 0;

  // TODO: Zero registers and RAM
  memset(cpu->reg, 0, sizeof cpu->reg);
  memset(cpu->ram, 0, sizeof cpu->ram);
  cpu->reg[SP] = 0xf4;
}

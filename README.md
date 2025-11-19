LC4 Simulator & Disassembler

This repository contains two related C projects for the LC4 architecture:

project-lc4-simulator: a cycle-accurate LC4 simulator that executes compiled .obj programs.

project-lc4-disassembler-whash: a disassembler that converts LC4 .obj files back into human-readable LC4 assembly.

Both projects were originally built for CIS 2400 (Computer Architecture) and are structured to work with LC4 .obj files produced by the course toolchain.

Repository Structure
LC4_Simulator_Assembler/
├── project-lc4-simulator/          # LC4 simulator (C)
│   ├── Makefile
│   ├── *.c / *.h                   # core simulator + loader
│   └── obj/                        # sample .obj programs (if provided)
├── project-lc4-disassembler-whash/ # LC4 disassembler (C)
│   ├── Makefile
│   ├── *.c / *.h                   # loader + hashtable + disassembler
│   └── obj/                        # sample .obj programs (if provided)
└── .DS_Store                       # macOS metadata (safe to ignore/remove)


You can work with each project independently by cd-ing into its directory.

Prerequisites

C compiler: gcc or clang

make

LC4 .obj files (e.g., compiled from LC4 assembly using the course tools)

Tested on macOS / Linux-style environments.

LC4 Simulator

The simulator:

Loads an LC4 .obj file into a simulated memory.

Tracks LC4 registers, program counter, condition codes, and privilege mode.

Decodes and executes instructions (arithmetic, logic, memory, control flow, traps).

Enforces basic privilege checks using code/data segment metadata (e.g., disallowing user mode from executing in data segments).

Optionally writes out an execution trace (PC + instruction + register/memory state) for debugging.

Build

From the project-lc4-simulator directory:

cd project-lc4-simulator
make            # builds the main simulator binary (e.g., lc4sim)


This should produce a simulator executable such as:

./lc4sim


(check the Makefile for the exact binary name if it differs).

Usage

Basic usage (single .obj file):

./lc4sim path/to/program.obj


Common behavior / features (depending on how you wired the project):

HALT: the simulator stops when it executes a HALT/RTI/illegal instruction as defined in the assignment.

Trace output: some versions write an execution trace file (e.g., trace.txt) showing each cycle’s PC and instruction.

Error handling: prints messages for privilege violations, invalid memory accesses, or unknown opcodes.

If your version takes extra flags (e.g., -t for trace), you can document them like:

./lc4sim -t trace.txt path/to/program.obj

LC4 Disassembler

The disassembler:

Parses LC4 .obj files (code, data, and symbol segments).

Stores them in a hashtable of linked lists keyed by memory address / segment.

Walks through the program segment and reverse-assembles each instruction into LC4 assembly mnemonics.

Outputs a readable .asm listing, including labels and data declarations.

Build

From the project-lc4-disassembler-whash directory:

cd project-lc4-disassembler-whash
make            # builds the disassembler binary (e.g., lc4dis)


This should produce a disassembler executable such as:

./lc4dis


(again, check the Makefile for the exact binary name).

Usage

Disassemble a single .obj file to stdout:

./lc4dis path/to/program.obj


Redirect to a file if you want an .asm output:

./lc4dis path/to/program.obj > program_disassembled.asm


Typical output will include:

Labels for addresses that appear in the symbol table.

Instructions decoded from opcodes (e.g., ADD, SUB, LDR, STR, BR, JSR, TRAP, etc.).

Data region printed as .DATA / .FILL or similar directives.

Implementation Notes (High-Level)

Simulator:

Implements a fetch–decode–execute loop.

Uses arrays or maps (e.g., isDATA, isCODE) to track memory segment types.

Handles LC4 instruction formats:

Arithmetic / logic (ADD, MUL, SUB, AND…)

Memory access (LDR, STR)

Control flow (JMP, JSR, BR, RTI)

TRAPs and constants (CONST/HICONST)

Updates condition codes and privilege mode according to the spec.

Disassembler:

Uses a loader to parse .obj headers and sections.

Stores instructions and data in a hashtable with separate chaining (linked lists) indexed by address.

For each program-memory node:

Decodes the 16-bit instruction into mnemonic + operands.

Formats an assembly string and associates labels where needed.

Acknowledgements

These projects were implemented as part of the CIS 2400: Computer Architecture course at the University of Pennsylvania, targeting the LC4 architecture used in the course.
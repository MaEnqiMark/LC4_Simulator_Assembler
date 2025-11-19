# LC4 Simulator & Disassembler

This repository contains two related C projects for the **LC4 architecture**:

- **`project-lc4-simulator`** — a cycle-accurate LC4 simulator that executes compiled `.obj` programs.  
- **`project-lc4-disassembler-whash`** — a disassembler that converts LC4 `.obj` files back into human-readable LC4 assembly.

Both projects were originally built for **CIS 2400: Computer Architecture** at the University of Pennsylvania.

---

## Repository Structure

```text
LC4_Simulator_Assembler/
├── project-lc4-simulator/           # LC4 simulator (C)
│   ├── Makefile
│   ├── *.c / *.h                    # core simulator + loader
│   └── obj/                         # sample .obj programs (if provided)
│
├── project-lc4-disassembler-whash/  # LC4 disassembler (C)
│   ├── Makefile
│   ├── *.c / *.h                    # loader + hashtable + disassembler
│   └── obj/                         # sample .obj programs (if provided)
│
└── .DS_Store                        # macOS metadata (safe to ignore/remove)

---

## Prerequisites

- C compiler: **gcc** or **clang**
- **make**
- LC4 `.obj` files (compiled using the CIS 2400 LC4 toolchain)

Tested on **macOS** and **Linux-style** environments.

---

# LC4 Simulator

## Build

```bash
cd project-lc4-simulator
make

This produces a simulator executable (e.g., lc4sim, depending on the Makefile).

Run
./lc4sim path/to/program.obj

Features

Loads an LC4 .obj file into simulated memory

Implements the fetch → decode → execute pipeline

Supports arithmetic, logic, memory, control flow, and trap instructions

Tracks registers, PC, condition codes, and privilege mode

Enforces segment-based privilege checks (isCODE, isDATA)

Optional execution trace output (if enabled)

LC4 Disassembler
Build
cd project-lc4-disassembler-whash
make

Run
./lc4dis path/to/program.obj

Features

Parses LC4 .obj headers and segments

Stores instructions and data using a hashtable with linked lists

Reverse-assembles instructions into readable LC4 assembly

Emits labels from the symbol table

Prints data using .FILL or .DATA directives

Implementation Details
Simulator Highlights

Fully decodes 16-bit LC4 instructions:

ADD, MUL, SUB, AND,

BR, JMP, JSR,

LDR, STR,

CONST, HICONST,

TRAP, RTI, etc.

Updates condition codes (N, Z, P)

Supports user/supervisor modes

Detects illegal instructions and out-of-bounds memory accesses

Disassembler Highlights

.obj loader parses:

CODE section

DATA section

SYMBOL table

Hashtable indexing on address for quick access

Outputs assembly strings for each program node

Connects symbols to correct addresses for labeling

Acknowledgements

These implementations were completed as part of CIS 2400: Computer Architecture at the University of Pennsylvania.
# pascal-compiler

This is an academic project implementing a Pascal-inspired interpreter using C++ and Flex. It features a recursive descent parser written manually (no Bison/Yacc) and supports basic Pascal constructs such as variable declarations, arithmetic expressions, and control flow.

## Features
- Manual recursive descent parser in C++
- Lexical analysis using Flex (`rules.l`)
- Support for:
  - Variable declarations
  - Arithmetic operations
  - `if`, `while`, and other control flow
- Organized test cases in the `test_cases/` folder

## How to Build and Run

1. Clean previous builds (optional but recommended):
   ```bash
   makeclean
2. Build the project
   ```bash
   make
3. Run the interpreter with a test file
   ```bash
   ./tips test_cases/factorial.pas


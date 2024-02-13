
# CustomLang

## Description

This repository hosts a custom-built compiler designed to parse and interpret a specialized programming language. The compiler is developed using C/C++ and integrates components like `ASTNode` for Abstract Syntax Tree representation and `SymbolTable` for symbol management. It uses Yacc/Bison for defining grammar and Lex for lexical analysis.

## Features

- Custom language parsing capability.
- Abstract Syntax Tree (AST) creation.
- Symbol table management for variables and function declarations.
- Use of Yacc/Bison and Lex for grammar and lexical analysis.
- Script (`compile.sh`) for streamlined compilation.
- Sample test program (`testProgram.txt`) to demonstrate compiler functionality.

## Getting Started

### Prerequisites

- C/C++ compiler
- Bison/Yacc
- Lex/Flex

### Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/ionutsavin/CustomLang.git
   ```
2. Change directory to the project folder:
   ```sh
   cd CustomLang
   ```

### Compilation

1. Change the permissions to make the file executable:
   ```bash
   chmod +x compile.sh
   ```

2. Compile the project:
   ```bash
   ./compiler.sh customLang
   ```

This script cleans previous builds, processes the grammar file with Bison and Lex, and compiles the source into an executable named `compiler`.

### Testing

To test the compiler, use the provided `testProgram.txt`:

```bash
./compiler < testProgram.txt
```

This runs the compiler against the test program, showcasing its parsing capabilities and symbol table generation.

### Symbol Table

The compiler outputs a symbol table file, detailing the variables and functions parsed from the source code. This file is essential for verifying the compiler's functionality.


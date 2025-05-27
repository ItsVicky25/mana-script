# Manascript Compiler

A modern compiler for the Manascript programming language, implemented in C++.

## Features

- Simple Finite-State Lexer
- Recursive Descent / LL(1) Parser
- Abstract Syntax Tree generation
- LLVM IR Code Generation
- Basic JIT Compilation using LLVM

## Project Structure

```
manascript/
├── CMakeLists.txt       # Build configuration
├── README.md            # Project documentation
├── include/             # Header files
│   ├── lexer.hpp        # Lexical analyzer
│   ├── parser.hpp       # Syntax analyzer
│   ├── ast.hpp          # Abstract Syntax Tree definitions
│   ├── transpiler.hpp   # Manascript to C++ transpiler
│   └── codegen.hpp      # LLVM IR code generator
├── src/                 # Source files
│   ├── main.cpp         # Main entry point
│   ├── lexer.cpp        # Lexer implementation
│   ├── parser.cpp       # Parser implementation
│   ├── ast.cpp          # AST implementation
│   └── transpiler.cpp   # Transpiler implementation
├── examples/            # Example programs
│   └── hello.mana       # Hello World in Manascript
├── output/              # Generated output files
│   └── hello.cpp        # Transpiled C++ code
├── tests/               # Test files
│   └── test_lexer.cpp   # Lexer tests
├── scripts/             # Build scripts
│   └── build.bat        # Windows build script
└── doc/                 # Documentation
    └── design.md        # Design document
```

## Building

### Prerequisites

- CMake 3.10 or later
- LLVM 10.0 or later
- C++17 compatible compiler

### Build Steps

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

```bash
# Compile and run Manascript program
./manascript examples/hello.mana

# Emit LLVM IR
./manascript --emit-llvm examples/hello.mana

# Transpile to C++
./manascript --emit-cpp examples/hello.mana

# JIT compile and run
./manascript --jit examples/hello.mana

# Dump tokens
./manascript --dump-tokens examples/hello.mana
```

## Language Features

Manascript is a simple, statically-typed programming language with a C-like syntax. It supports:

- Variables and constants
- Basic data types: integers, floats, booleans, strings
- Functions with parameters and return values
- Control flow statements: if/else, while loops
- Arithmetic and logical operators
- Comments (line and block)

## Example Program

```javascript
// Hello World program in Manascript

function main() {
    print("Hello, World!");
    return 0;
}
```

## License

This project is open source under the MIT license.
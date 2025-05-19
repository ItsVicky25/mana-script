# Manascript Language Design

This document outlines the design of the Manascript language and its compiler.

## 1. Overview

Manascript is a simple, statically-typed programming language with a C-like syntax. The compiler is implemented in C++ and uses LLVM for code generation and optimization.

## 2. Compiler Architecture

The compiler follows a traditional multi-pass design:

1. **Lexical Analysis**: Convert source code into tokens
2. **Syntax Analysis**: Parse tokens into an Abstract Syntax Tree (AST)
3. **Semantic Analysis**: Perform type checking and symbol resolution
4. **Code Generation**: Generate LLVM IR from the AST
5. **Optimization**: Apply LLVM optimization passes
6. **Execution**: JIT compile or output to file

### 2.1 Lexical Analysis

The lexer is implemented as a simple finite-state machine that reads the source code character-by-character and groups them into tokens. It handles:

- Keywords
- Identifiers
- Literals (integers, floats, strings, booleans)
- Operators
- Punctuation
- Comments
- Whitespace

### 2.2 Syntax Analysis

The parser is implemented as a recursive descent parser (LL(1)) that constructs an AST from the token stream. It handles:

- Expressions
- Statements
- Declarations
- Control flow
- Functions

### 2.3 Abstract Syntax Tree

The AST is a hierarchical representation of the program structure. It uses a visitor pattern for traversing and transforming the tree.

### 2.4 Code Generation

The code generator traverses the AST and generates LLVM IR. It handles:

- Variables and constants
- Expressions and operators
- Control flow
- Functions
- External function calls

### 2.5 JIT Compilation

The JIT compiler uses LLVM's ORC JIT API to compile and execute the generated IR at runtime.

## 3. Language Features

### 3.1 Types

Manascript supports the following basic types:

- `int`: 32-bit signed integers
- `float`: 64-bit floating point numbers
- `bool`: Boolean values (true/false)
- `string`: Text strings
- `void`: No value

### 3.2 Variables and Constants

Variables are declared using the `var` keyword, and constants with the `const` keyword:

```javascript
var x = 42;
const PI = 3.14159;
```

### 3.3 Functions

Functions are declared using the `function` keyword:

```javascript
function add(a, b) {
    return a + b;
}
```

### 3.4 Control Flow

Manascript supports the following control flow statements:

- `if`/`else` for conditional execution
- `while` for loops
- `return` for function returns
- `break` and `continue` for loop control

```javascript
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}
```

### 3.5 Operators

Manascript supports the following operators:

- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Logical: `&&`, `||`, `!`
- Assignment: `=`

## 4. Future Enhancements

- Type inference
- First-class functions
- Arrays and collections
- Modules and imports
- Garbage collection
- Concurrency support
- Object-oriented features
- Pattern matching
- Custom operators
- Generics

## 5. Implementation Challenges

### 5.1 Error Handling

The compiler provides detailed error messages with source location information to help developers identify and fix issues.

### 5.2 Optimization

The compiler leverages LLVM's optimization passes to generate efficient code.

### 5.3 Cross-Platform Support

The compiler is designed to be platform-independent, supporting Linux, macOS, and Windows.

## 6. Conclusion

Manascript is designed to be a simple, yet powerful language for learning compiler construction and exploring language design concepts. The LLVM backend provides efficient code generation and execution, while the modular architecture allows for easy extension and modification.

Future work will focus on expanding the language features, improving the development experience, and optimizing performance.
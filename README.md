# ManaScript Interpreter

A lightweight, modern scripting language interpreter written in C++. ManaScript is designed to be simple yet powerful, with a clean syntax and robust features.

## Features

- Modern C++ implementation
- Clean and intuitive syntax
- Support for basic arithmetic operations
- Variable declarations and assignments
- Control flow statements (if/else)
- Print statements for output
- Interactive mode
- File execution mode
- Tokenization and parsing
- Abstract Syntax Tree (AST) generation

## Complete Setup Guide

### Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- Visual Studio 2022 (for Windows)
- PowerShell (for Windows build script)

### Step-by-Step Setup

1. **Install Required Tools**
   - Install Visual Studio 2022 with C++ development tools
   - Install CMake from [cmake.org](https://cmake.org/download/)
   - Ensure PowerShell is available (comes with Windows 10/11)

2. **Clone the Repository**
```bash
git clone https://github.com/yourusername/mana-script.git
cd mana-script
```

3. **Build the Project**
   - On Windows:
```powershell
# Run the build script
./build.ps1

# The script will:
# 1. Create a build directory
# 2. Configure CMake
# 3. Build the project
# 4. Create a bin directory with the executable
```

   - On Linux/macOS:
```bash
# Create and enter build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Create bin directory and copy executable
mkdir -p ../bin
cp manascript ../bin/
```

4. **Verify Installation**
```bash
# Check if the executable exists
ls bin/manascript.exe  # Windows
ls bin/manascript      # Linux/macOS

# Run version check
./bin/manascript.exe -v  # Windows
./bin/manascript -v      # Linux/macOS
```

## Running ManaScript

### 1. Interactive Mode
```bash
# Start the interpreter in interactive mode
./bin/manascript.exe -i  # Windows
./bin/manascript -i      # Linux/macOS

# Example interactive session:
> let x = 10
> let y = 20
> print(x + y)
30
> exit
```

### 2. Running Script Files
Create a file named `hello.ms`:
```javascript
// hello.ms
let name = "World";
print("Hello, " + name + "!");
let sum = 5 + 3;
print("5 + 3 = " + sum);
```

Run the script:
```bash
./bin/manascript.exe hello.ms  # Windows
./bin/manascript hello.ms      # Linux/macOS
```

### 3. Command Line Execution
```bash
# Execute code directly from command line
./bin/manascript.exe -e "let x = 5; print(x * 2);"  # Windows
./bin/manascript -e "let x = 5; print(x * 2);"      # Linux/macOS
```

### 4. Running Tests
```bash
# Run the test suite
./build/tests/Release/test_lexer.exe  # Windows
./build/tests/test_lexer              # Linux/macOS
```

### 5. Example Scripts
The `examples` directory contains sample scripts:
```bash
# Run an example script
./bin/manascript.exe examples/arithmetic.ms  # Windows
./bin/manascript examples/arithmetic.ms      # Linux/macOS
```

## Troubleshooting

### Common Issues

1. **Build Fails**
   - Ensure CMake is properly installed
   - Check if Visual Studio 2022 is installed with C++ tools
   - Verify PowerShell execution policy allows script execution

2. **Executable Not Found**
   - Check if the `bin` directory exists
   - Verify the build process completed successfully
   - Ensure you're in the correct directory

3. **Script Execution Errors**
   - Check script syntax
   - Verify file permissions
   - Ensure file extension is `.ms`

## Language Features

### Variables
```javascript
let x = 10;
let name = "John";
```

### Arithmetic Operations
```javascript
let sum = 5 + 3;
let product = 4 * 2;
let difference = 10 - 5;
let quotient = 15 / 3;
```

### Control Flow
```javascript
if (x < y) {
    print("x is less than y");
} else {
    print("x is greater than or equal to y");
}
```

### Print Statements
```javascript
print("Hello, World!");
print(42);
```

## Project Structure

```
mana-script/
├── bin/            # Compiled executables
├── include/        # Header files
├── src/           # Source files
├── tests/         # Test files
├── examples/      # Example scripts
├── CMakeLists.txt # CMake build configuration
└── build.ps1      # Windows build script
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Thanks to all contributors who have helped shape this project
- Inspired by modern scripting languages like JavaScript and Python 
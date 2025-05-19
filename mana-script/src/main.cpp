/**
 * @file main.cpp
 * @brief Main entry point for the ManaScript interpreter
 */

#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "transpiler.hpp"
#include "error.hpp"
#include "token.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>

namespace mana {

void printUsage() {
    std::cout << "ManaScript Interpreter v0.1.0\n"
              << "Usage:\n"
              << "  manascript [options] [file]\n\n"
              << "Options:\n"
              << "  -h, --help     Show this help message\n"
              << "  -v, --version  Show version information\n"
              << "  -i, --interactive  Start interactive mode\n"
              << "  -t, --tokenize Show tokenized output\n\n"
              << "Examples:\n"
              << "  manascript script.ms        Run a script file\n"
              << "  manascript -i              Start interactive mode\n"
              << "  manascript -t script.ms    Show tokenized output\n";
}

void printVersion() {
    std::cout << "ManaScript Interpreter v0.1.0\n"
              << "Copyright (c) 2024\n";
}

void printTokens(const std::vector<Token>& tokens) {
    std::cout << "\nTokenized output:\n";
    std::cout << "----------------\n";
    for (const auto& token : tokens) {
        std::cout << "Line " << token.line << ", Col " << token.column << ": ";
        std::cout << "Type: " << static_cast<int>(token.type) << ", ";
        std::cout << "Lexeme: '" << token.lexeme << "'\n";
    }
    std::cout << "----------------\n";
}

void runInteractiveMode() {
    std::cout << "ManaScript Interactive Mode\n"
              << "Type 'exit' or 'quit' to exit\n"
              << "Type 'help' for help\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (line == "exit" || line == "quit") {
            break;
        }
        if (line == "help") {
            std::cout << "Available commands:\n"
                      << "  exit, quit  Exit the interpreter\n"
                      << "  help       Show this help message\n"
                      << "  clear      Clear the screen\n";
            continue;
        }
        if (line == "clear") {
            #ifdef _WIN32
            system("cls");
            #else
            system("clear");
            #endif
            continue;
        }
        if (line.empty()) {
            continue;
        }

        try {
            Lexer lexer(line);
            auto tokens = lexer.scanTokens();
            printTokens(tokens);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}

void runFile(const std::string& filename, bool showTokens) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "'\n";
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        Lexer lexer(content, filename);
        auto tokens = lexer.scanTokens();

        if (showTokens) {
            printTokens(tokens);
        } else {
            // TODO: Add parser and interpreter here
            std::cout << "Running script: " << filename << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

} // namespace mana

int main(int argc, char* argv[]) {
    if (argc < 2) {
        mana::printUsage();
        return 1;
    }

    std::string arg = argv[1];
    
    if (arg == "-h" || arg == "--help") {
        mana::printUsage();
        return 0;
    }
    
    if (arg == "-v" || arg == "--version") {
        mana::printVersion();
        return 0;
    }
    
    if (arg == "-i" || arg == "--interactive") {
        mana::runInteractiveMode();
        return 0;
    }
    
    bool showTokens = false;
    if (arg == "-t" || arg == "--tokenize") {
        showTokens = true;
        if (argc < 3) {
            std::cerr << "Error: No input file specified\n";
            return 1;
        }
        mana::runFile(argv[2], showTokens);
        return 0;
    }
    
    // If no special flags, treat as a file
    mana::runFile(arg, showTokens);
    return 0;
}
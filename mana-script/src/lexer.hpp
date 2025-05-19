/**
 * @file lexer.hpp
 * @brief Lexical analyzer for the ManaScript language
 */

#pragma once

#include "token.hpp"
#include "error.hpp"
#include <string>
#include <vector>

namespace mana {

/**
 * @class Lexer
 * @brief Converts source code into a sequence of tokens
 * 
 * The Lexer class is responsible for breaking down the input source code
 * into a sequence of tokens that can be processed by the parser. It handles:
 * - Keywords and identifiers
 * - Operators and punctuation
 * - String literals with escape sequences
 * - Numeric literals (decimal, hex, binary, scientific notation)
 * - Comments (line and block)
 * - Error reporting
 */
class Lexer {
public:
    /**
     * @brief Constructs a new Lexer
     * @param source The source code to tokenize
     * @param filename The name of the source file (for error reporting)
     */
    Lexer(const std::string& source, const std::string& filename = "");

    /**
     * @brief Scans the source code and returns a sequence of tokens
     * @return Vector of tokens representing the source code
     */
    std::vector<Token> scanTokens();

private:
    std::string source;      ///< The source code to tokenize
    std::string filename;    ///< Name of the source file
    std::vector<Token> tokens;  ///< The tokens generated from the source
    int start = 0;          ///< Start of the current token
    int current = 0;        ///< Current position in the source
    int line = 1;           ///< Current line number
    int column = 1;         ///< Current column number

    /**
     * @brief Checks if we've reached the end of the source
     * @return true if at end of source, false otherwise
     */
    bool isAtEnd() const;

    /**
     * @brief Advances the current position and returns the character
     * @return The character at the current position
     */
    char advance();

    /**
     * @brief Returns the character at the current position without advancing
     * @return The character at the current position
     */
    char peek() const;

    /**
     * @brief Returns the character after the current position without advancing
     * @return The character after the current position
     */
    char peekNext() const;

    /**
     * @brief Checks if the current character matches the expected character
     * @param expected The character to match
     * @return true if matched, false otherwise
     */
    bool match(char expected);

    /**
     * @brief Adds a token with the current lexeme
     * @param type The type of token to add
     */
    void addToken(TokenType type);

    /**
     * @brief Adds a token with a specific lexeme
     * @param type The type of token to add
     * @param lexeme The lexeme for the token
     */
    void addToken(TokenType type, const std::string& lexeme);

    /**
     * @brief Scans a single token from the source
     */
    void scanToken();

    /**
     * @brief Scans a string literal
     * Handles escape sequences: \n, \t, \r, \", \\
     */
    void scanString();

    /**
     * @brief Scans a number literal
     * Supports:
     * - Decimal integers (e.g., 42)
     * - Floating-point numbers (e.g., 3.14)
     * - Hexadecimal numbers (e.g., 0x1A)
     * - Binary numbers (e.g., 0b1010)
     * - Scientific notation (e.g., 1.23e-4)
     */
    void scanNumber();

    /**
     * @brief Scans an identifier or keyword
     */
    void scanIdentifier();

    /**
     * @brief Reports a lexical error
     * @param message The error message
     */
    void reportError(const std::string& message);

    /**
     * @brief Gets the current source location
     * @return SourceLocation object with current line and column
     */
    SourceLocation getCurrentLocation() const;

    /**
     * @brief Gets the context of the current line
     * @return The current line of source code
     */
    std::string getLineContext() const;
};

} // namespace mana 
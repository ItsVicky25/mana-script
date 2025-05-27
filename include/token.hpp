#ifndef MANASCRIPT_TOKEN_HPP
#define MANASCRIPT_TOKEN_HPP

#include <string>
#include <unordered_map>
#include <iostream>

namespace mana {

/**
 * @brief Represents token types in the Manascript language
 */
enum class TokenType {
    // Special tokens
    END_OF_FILE,
    ERROR,
    
    // Literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    BOOL_LITERAL,
    
    // Keywords
    FUNCTION,
    VAR,
    CONST,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    BREAK,
    CONTINUE,
    TRUE,
    FALSE,
    NIL,
    
    // Operators
    PLUS,          // +
    MINUS,         // -
    STAR,          // *
    SLASH,         // /
    PERCENT,       // %
    
    EQUAL,         // =
    EQUAL_EQUAL,   // ==
    BANG,          // !
    BANG_EQUAL,    // !=
    LESS,          // <
    LESS_EQUAL,    // <=
    GREATER,       // >
    GREATER_EQUAL, // >=
    
    AND,           // &&
    OR,            // ||
    
    // Punctuation
    DOT,           // .
    COMMA,         // ,
    SEMICOLON,     // ;
    COLON,         // :
    LEFT_PAREN,    // (
    RIGHT_PAREN,   // )
    LEFT_BRACE,    // {
    RIGHT_BRACE,   // }
    LEFT_BRACKET,  // [
    RIGHT_BRACKET, // ]
};

/**
 * @brief Represents a Token in the Manascript language
 */
class Token {
public:
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
    
    std::string toString() const;
};

/**
 * @brief Converts keyword strings to their corresponding token types
 */
class Keywords {
private:
    static std::unordered_map<std::string, TokenType> keywords;
    
public:
    static TokenType getKeyword(const std::string& text);
    static bool isKeyword(const std::string& text);
};

// Utility functions for token operations
std::string tokenTypeToString(TokenType type);
std::ostream& operator<<(std::ostream& os, const Token& token);

} // namespace mana

#endif // MANASCRIPT_TOKEN_HPP
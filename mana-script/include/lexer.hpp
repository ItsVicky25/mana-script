#ifndef MANASCRIPT_LEXER_HPP
#define MANASCRIPT_LEXER_HPP

#include "token.hpp"
#include "error.hpp"
#include <string>
#include <vector>
#include <memory>

namespace mana {

/**
 * @brief Lexical analyzer for Manascript
 * 
 * Converts source code into a stream of tokens using a finite-state machine approach.
 */
class Lexer {
private:
    std::string source;
    std::string filename;
    std::vector<Token> tokens;
    
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;
    
    // Helper methods
    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);
    
    // Token scanners
    void scanToken();
    void scanString();
    void scanNumber();
    void scanIdentifier();
    
    // Error handling
    void reportError(const std::string& message);
    
    // Source position tracking
    SourceLocation getCurrentLocation() const;
    std::string getLineContext() const;

public:
    Lexer(const std::string& source, const std::string& filename = "");
    
    /**
     * @brief Scans the source code and generates tokens
     * @return Vector of tokens
     */
    std::vector<Token> scanTokens();
    
    /**
     * @brief Returns the tokens that have been scanned so far
     */
    const std::vector<Token>& getTokens() const { return tokens; }
};

} // namespace mana

#endif // MANASCRIPT_LEXER_HPP
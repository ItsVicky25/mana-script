#ifndef MANASCRIPT_PARSER_HPP
#define MANASCRIPT_PARSER_HPP

#include "token.hpp"
#include "ast.hpp"
#include "error.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <functional>

namespace mana {

/**
 * @brief Exception thrown by the parser when a syntax error is encountered
 */
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Recursive Descent Parser for Manascript
 */
class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;
    int max_params = 255;  // Maximum number of parameters in a function
    std::string filename;
    
    // Helper methods
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    
    // Error handling
    ParseError error(const Token& token, const std::string& message);
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    
    // Recursive descent parsing methods
    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr logicalOr();
    ExprPtr logicalAnd();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr call();
    ExprPtr primary();
    
    StmtPtr declaration();
    StmtPtr varDeclaration(bool is_const = false);
    StmtPtr functionDeclaration();
    StmtPtr statement();
    StmtPtr expressionStatement();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr returnStatement();
    StmtPtr blockStatement();
    
    // Parsing utilities
    ExprPtr finishCall(ExprPtr callee);
    
public:
    Parser(const std::vector<Token>& tokens, const std::string& filename = "");
    
    /**
     * @brief Parse the tokens into an AST
     * @return Vector of statements
     */
    std::vector<StmtPtr> parse();
};

} // namespace mana

#endif // MANASCRIPT_PARSER_HPP
#include "parser.hpp"

namespace mana {

Parser::Parser(const std::vector<Token>& tokens, const std::string& filename)
    : tokens(tokens), filename(filename) {}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (const std::exception& e) {
            synchronize();
        }
    }
    return statements;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

ParseError Parser::error(const Token& token, const std::string& message) {
    SourceLocation location(filename, token.line, token.column);
    
    if (token.type == TokenType::END_OF_FILE) {
        diagnostics.report(DiagnosticSeverity::ERROR, message + " at end of file", location);
    } else {
        diagnostics.report(DiagnosticSeverity::ERROR, 
                          message + " at '" + token.lexeme + "'", location);
    }
    
    return ParseError(message);
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    throw error(peek(), message);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::FUNCTION:
            case TokenType::VAR:
            case TokenType::CONST:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

ExprPtr Parser::expression() {
    return assignment();
}

ExprPtr Parser::assignment() {
    ExprPtr expr = logicalOr();
    
    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        ExprPtr value = assignment();
        
        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            Token name = varExpr->getName();
            return std::make_shared<AssignExpr>(name, value);
        }
        
        error(equals, "Invalid assignment target");
    }
    
    return expr;
}

ExprPtr Parser::logicalOr() {
    ExprPtr expr = logicalAnd();
    
    while (match(TokenType::OR)) {
        Token op = previous();
        ExprPtr right = logicalAnd();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::logicalAnd() {
    ExprPtr expr = equality();
    
    while (match(TokenType::AND)) {
        Token op = previous();
        ExprPtr right = equality();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::equality() {
    ExprPtr expr = comparison();
    
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        ExprPtr right = comparison();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::comparison() {
    ExprPtr expr = term();
    
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, 
                  TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        ExprPtr right = term();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::term() {
    ExprPtr expr = factor();
    
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        ExprPtr right = factor();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::factor() {
    ExprPtr expr = unary();
    
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        Token op = previous();
        ExprPtr right = unary();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    
    return expr;
}

ExprPtr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        ExprPtr right = unary();
        return std::make_shared<UnaryExpr>(op, right);
    }
    
    return call();
}

ExprPtr Parser::call() {
    ExprPtr expr = primary();
    
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(expr);
        } else {
            break;
        }
    }
    
    return expr;
}

ExprPtr Parser::finishCall(ExprPtr callee) {
    std::vector<ExprPtr> arguments;
    
    // Parse arguments
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                error(peek(), "Cannot have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    
    return std::make_shared<CallExpr>(callee, paren, arguments);
}

ExprPtr Parser::primary() {
    if (match(TokenType::FALSE)) return std::make_shared<LiteralExpr>(false);
    if (match(TokenType::TRUE)) return std::make_shared<LiteralExpr>(true);
    if (match(TokenType::NIL)) return std::make_shared<LiteralExpr>(nullptr);

    if (match(TokenType::INTEGER_LITERAL) || match(TokenType::FLOAT_LITERAL)) {
        double value = std::stod(previous().lexeme);
        return std::make_shared<LiteralExpr>(value);
    }

    if (match(TokenType::STRING_LITERAL)) {
        std::string value = previous().lexeme;
        return std::make_shared<LiteralExpr>(value);
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_shared<VariableExpr>(previous());
    }

    if (match(TokenType::LEFT_PAREN)) {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<GroupingExpr>(expr);
    }

    throw error(peek(), "Expect expression.");
}

StmtPtr Parser::declaration() {
    if (match(TokenType::VAR)) return varDeclaration();
    return statement();
}

StmtPtr Parser::statement() {
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::PRINT)) return printStatement();
    if (match(TokenType::RETURN)) return returnStatement();
    if (match(TokenType::LEFT_BRACE)) return std::make_shared<BlockStmt>(block());
    return expressionStatement();
}

StmtPtr Parser::printStatement() {
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_shared<ExpressionStmt>(value);
}

StmtPtr Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");
    
    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }
    
    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

StmtPtr Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition");
    
    StmtPtr body = statement();
    
    return std::make_shared<WhileStmt>(condition, body);
}

StmtPtr Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'");
    
    StmtPtr initializer;
    if (match(TokenType::SEMICOLON)) {
        initializer = nullptr;
    } else if (match(TokenType::VAR)) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }
    
    ExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition");
    
    ExprPtr increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses");
    
    StmtPtr body = statement();
    
    if (increment != nullptr) {
        body = std::make_shared<BlockStmt>(std::vector<StmtPtr>{
            body,
            std::make_shared<ExpressionStmt>(increment)
        });
    }
    
    if (condition == nullptr) {
        condition = std::make_shared<LiteralExpr>(true);
    }
    body = std::make_shared<WhileStmt>(condition, body);
    
    if (initializer != nullptr) {
        body = std::make_shared<BlockStmt>(std::vector<StmtPtr>{
            initializer,
            body
        });
    }
    
    return body;
}

StmtPtr Parser::returnStatement() {
    Token keyword = previous();
    ExprPtr value = nullptr;
    
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expect ';' after return value");
    return std::make_shared<ReturnStmt>(keyword, value);
}

StmtPtr Parser::expressionStatement() {
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression");
    return std::make_shared<ExpressionStmt>(expr);
}

StmtPtr Parser::varDeclaration(bool is_const) {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name");
    
    ExprPtr initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    } else if (is_const) {
        throw error(name, "Const declarations must have an initializer");
    }
    
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");
    return std::make_shared<VarDeclStmt>(name, initializer, is_const);
}

std::vector<StmtPtr> Parser::block() {
    std::vector<StmtPtr> statements;
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
    return statements;
}

} // namespace mana
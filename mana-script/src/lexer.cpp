#include "../include/lexer.hpp"
#include "../include/token.hpp"
#include <cctype>
#include <sstream>
#include <unordered_map>

namespace mana {

Lexer::Lexer(const std::string& source, const std::string& filename)
    : source(source), filename(filename) {}

std::vector<Token> Lexer::scanTokens() {
    tokens.clear();
    start = 0;
    current = 0;
    line = 1;
    column = 1;
    
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    
    // Add EOF token
    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

bool Lexer::isAtEnd() const {
    return current >= static_cast<int>(source.length());
}

char Lexer::advance() {
    column++;
    return source[current++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() const {
    if (current + 1 >= static_cast<int>(source.length())) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    
    current++;
    column++;
    return true;
}

void Lexer::addToken(TokenType type) {
    std::string lexeme = source.substr(start, current - start);
    tokens.emplace_back(type, lexeme, line, column - lexeme.length());
}

void Lexer::addToken(TokenType type, const std::string& lexeme) {
    tokens.emplace_back(type, lexeme, line, column - lexeme.length());
}

void Lexer::scanToken() {
    char c = advance();
    
    switch (c) {
        // Single-character tokens
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case ':': addToken(TokenType::COLON); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '*': addToken(TokenType::STAR); break;
        case '%': addToken(TokenType::PERCENT); break;
        
        // One or two character tokens
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
        
        // Logical operators
        case '&': 
            if (match('&')) {
                addToken(TokenType::AND);
            } else {
                reportError("Unexpected character '&'");
            }
            break;
        case '|': 
            if (match('|')) {
                addToken(TokenType::OR);
            } else {
                reportError("Unexpected character '|'");
            }
            break;
        
        // Division or comment
        case '/':
            if (match('/')) {
                // Single-line comment
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
            } else if (match('*')) {
                // Multi-line comment
                int nesting = 1;
                while (nesting > 0 && !isAtEnd()) {
                    if (peek() == '/' && peekNext() == '*') {
                        advance();
                        advance();
                        nesting++;
                    } else if (peek() == '*' && peekNext() == '/') {
                        advance();
                        advance();
                        nesting--;
                    } else if (peek() == '\n') {
                        advance();
                        line++;
                        column = 1;
                    } else {
                        advance();
                    }
                }
                
                if (nesting > 0) {
                    reportError("Unterminated comment");
                }
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        
        // Whitespace
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;
        
        case '\n':
            line++;
            column = 1;
            break;
        
        // String literals
        case '"': scanString(); break;
        
        // Number literals and other tokens
        default:
            if (std::isdigit(c)) {
                scanNumber();
            } else if (std::isalpha(c) || c == '_') {
                scanIdentifier();
            } else {
                reportError(std::string("Unexpected character '") + c + "'");
            }
            break;
    }
}

void Lexer::scanString() {
    std::string value;
    
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            column = 1;
        }
        
        // Handle escape sequences
        if (peek() == '\\' && !isAtEnd()) {
            advance(); // Consume backslash
            
            switch (peek()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default:
                    reportError(std::string("Invalid escape sequence '\\") + peek() + "'");
                    break;
            }
            advance();
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        reportError("Unterminated string");
        return;
    }
    
    // Consume the closing quote
    advance();
    
    // Add the string token with the processed value
    addToken(TokenType::STRING_LITERAL, value);
}

void Lexer::scanNumber() {
    // Check for hexadecimal
    if (peek() == 'x' && current - start == 1 && source[start] == '0') {
        advance(); // Consume 'x'
        bool hasDigits = false;
        while (std::isxdigit(peek())) {
            advance();
            hasDigits = true;
        }
        if (!hasDigits) {
            reportError("Invalid hexadecimal number");
            return;
        }
        addToken(TokenType::INTEGER_LITERAL);
        return;
    }
    
    // Check for binary
    if (peek() == 'b' && current - start == 1 && source[start] == '0') {
        advance(); // Consume 'b'
        bool hasDigits = false;
        while (peek() == '0' || peek() == '1') {
            advance();
            hasDigits = true;
        }
        if (!hasDigits) {
            reportError("Invalid binary number");
            return;
        }
        addToken(TokenType::INTEGER_LITERAL);
        return;
    }
    
    // Handle decimal numbers
    while (std::isdigit(peek())) {
        advance();
    }
    
    // Look for a decimal part
    if (peek() == '.' && std::isdigit(peekNext())) {
        // Consume the dot
        advance();
        
        // Consume decimal digits
        while (std::isdigit(peek())) {
            advance();
        }
        
        // Check for scientific notation
        if (peek() == 'e' || peek() == 'E') {
            advance(); // Consume 'e' or 'E'
            
            // Optional sign
            if (peek() == '+' || peek() == '-') {
                advance();
            }
            
            // Must have at least one digit after 'e'
            if (!std::isdigit(peek())) {
                reportError("Invalid scientific notation");
                return;
            }
            
            // Consume exponent digits
            while (std::isdigit(peek())) {
                advance();
            }
        }
        
        addToken(TokenType::FLOAT_LITERAL);
    } else {
        addToken(TokenType::INTEGER_LITERAL);
    }
}

void Lexer::scanIdentifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }
    
    // Check if the identifier is a keyword
    std::string text = source.substr(start, current - start);
    TokenType type = Keywords::getKeyword(text);
    
    if (type == TokenType::TRUE || type == TokenType::FALSE) {
        addToken(TokenType::BOOL_LITERAL, text);
    } else {
        addToken(type);
    }
}

void Lexer::reportError(const std::string& message) {
    SourceLocation location = getCurrentLocation();
    std::string context = getLineContext();
    
    diagnostics.report(
        DiagnosticSeverity::ERROR,
        message,
        location,
        context
    );
    
    // Add an error token
    tokens.emplace_back(TokenType::ERROR, message, line, column);
}

SourceLocation Lexer::getCurrentLocation() const {
    return SourceLocation(filename, line, column);
}

std::string Lexer::getLineContext() const {
    // Find the start of the current line
    int lineStart = start;
    while (lineStart > 0 && source[lineStart - 1] != '\n') {
        lineStart--;
    }
    
    // Find the end of the current line
    int lineEnd = current;
    while (lineEnd < static_cast<int>(source.length()) && source[lineEnd] != '\n') {
        lineEnd++;
    }
    
    return source.substr(lineStart, lineEnd - lineStart);
}

} // namespace mana
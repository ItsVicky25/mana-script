#ifndef MANASCRIPT_AST_HPP
#define MANASCRIPT_AST_HPP

#include "token.hpp"
#include <memory>
#include <vector>
#include <string>
#include <variant>

namespace mana {

// Forward declarations
class Expression;
class Statement;
class AstVisitor;

using ExprPtr = std::shared_ptr<Expression>;
using StmtPtr = std::shared_ptr<Statement>;

/**
 * @brief Base class for all AST nodes
 */
class AstNode {
public:
    virtual ~AstNode() = default;
};

/**
 * @brief Base class for all expressions in the AST
 */
class Expression : public AstNode {
public:
    virtual ~Expression() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};

/**
 * @brief Base class for all statements in the AST
 */
class Statement : public AstNode {
public:
    virtual ~Statement() = default;
    virtual void accept(AstVisitor& visitor) = 0;
};

/**
 * @brief Visitor interface for the AST
 */
class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    
    // Expression visitors
    virtual void visitLiteralExpr(class LiteralExpr& expr) = 0;
    virtual void visitUnaryExpr(class UnaryExpr& expr) = 0;
    virtual void visitBinaryExpr(class BinaryExpr& expr) = 0;
    virtual void visitGroupingExpr(class GroupingExpr& expr) = 0;
    virtual void visitVariableExpr(class VariableExpr& expr) = 0;
    virtual void visitAssignExpr(class AssignExpr& expr) = 0;
    virtual void visitCallExpr(class CallExpr& expr) = 0;
    
    // Statement visitors
    virtual void visitExpressionStmt(class ExpressionStmt& stmt) = 0;
    virtual void visitVarDeclStmt(class VarDeclStmt& stmt) = 0;
    virtual void visitBlockStmt(class BlockStmt& stmt) = 0;
    virtual void visitIfStmt(class IfStmt& stmt) = 0;
    virtual void visitWhileStmt(class WhileStmt& stmt) = 0;
    virtual void visitFunctionStmt(class FunctionStmt& stmt) = 0;
    virtual void visitReturnStmt(class ReturnStmt& stmt) = 0;
};

/**
 * @brief Represents a literal value (number, string, boolean, nil)
 */
class LiteralExpr : public Expression {
public:
    using LiteralValue = std::variant<int, double, std::string, bool, std::nullptr_t>;
    
    LiteralExpr(const LiteralValue& value) : value(value) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitLiteralExpr(*this);
    }
    
    const LiteralValue& getValue() const { return value; }
    
private:
    LiteralValue value;
};

/**
 * @brief Represents a unary operation (e.g., -x, !x)
 */
class UnaryExpr : public Expression {
public:
    UnaryExpr(Token op, ExprPtr right)
        : op(op), right(right) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitUnaryExpr(*this);
    }
    
    const Token& getOperator() const { return op; }
    ExprPtr getRight() const { return right; }
    
private:
    Token op;
    ExprPtr right;
};

/**
 * @brief Represents a binary operation (e.g., a + b, a < b)
 */
class BinaryExpr : public Expression {
public:
    BinaryExpr(ExprPtr left, Token op, ExprPtr right)
        : left(left), op(op), right(right) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitBinaryExpr(*this);
    }
    
    ExprPtr getLeft() const { return left; }
    const Token& getOperator() const { return op; }
    ExprPtr getRight() const { return right; }
    
private:
    ExprPtr left;
    Token op;
    ExprPtr right;
};

/**
 * @brief Represents a grouping expression (e.g., (a + b))
 */
class GroupingExpr : public Expression {
public:
    GroupingExpr(ExprPtr expression)
        : expression(expression) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitGroupingExpr(*this);
    }
    
    ExprPtr getExpression() const { return expression; }
    
private:
    ExprPtr expression;
};

/**
 * @brief Represents a variable reference
 */
class VariableExpr : public Expression {
public:
    VariableExpr(Token name)
        : name(name) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitVariableExpr(*this);
    }
    
    const Token& getName() const { return name; }
    
private:
    Token name;
};

/**
 * @brief Represents an assignment expression (e.g., a = 5)
 */
class AssignExpr : public Expression {
public:
    AssignExpr(Token name, ExprPtr value)
        : name(name), value(value) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitAssignExpr(*this);
    }
    
    const Token& getName() const { return name; }
    ExprPtr getValue() const { return value; }
    
private:
    Token name;
    ExprPtr value;
};

/**
 * @brief Represents a function call (e.g., foo(a, b))
 */
class CallExpr : public Expression {
public:
    CallExpr(ExprPtr callee, Token paren, std::vector<ExprPtr> arguments)
        : callee(callee), paren(paren), arguments(arguments) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitCallExpr(*this);
    }
    
    ExprPtr getCallee() const { return callee; }
    const Token& getParen() const { return paren; }
    const std::vector<ExprPtr>& getArguments() const { return arguments; }
    
private:
    ExprPtr callee;
    Token paren;  // Right parenthesis token, used for error reporting
    std::vector<ExprPtr> arguments;
};

/**
 * @brief Represents an expression statement
 */
class ExpressionStmt : public Statement {
public:
    ExpressionStmt(ExprPtr expression)
        : expression(expression) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitExpressionStmt(*this);
    }
    
    ExprPtr getExpression() const { return expression; }
    
private:
    ExprPtr expression;
};

/**
 * @brief Represents a variable declaration (e.g., var x = 5)
 */
class VarDeclStmt : public Statement {
public:
    VarDeclStmt(Token name, ExprPtr initializer = nullptr, bool is_const = false)
        : name(name), initializer(initializer), is_const(is_const) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitVarDeclStmt(*this);
    }
    
    const Token& getName() const { return name; }
    ExprPtr getInitializer() const { return initializer; }
    bool isConst() const { return is_const; }
    
private:
    Token name;
    ExprPtr initializer;
    bool is_const;
};

/**
 * @brief Represents a block statement (e.g., { stmt1; stmt2; })
 */
class BlockStmt : public Statement {
public:
    BlockStmt(std::vector<StmtPtr> statements)
        : statements(statements) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitBlockStmt(*this);
    }
    
    const std::vector<StmtPtr>& getStatements() const { return statements; }
    
private:
    std::vector<StmtPtr> statements;
};

/**
 * @brief Represents an if statement
 */
class IfStmt : public Statement {
public:
    IfStmt(ExprPtr condition, StmtPtr then_branch, StmtPtr else_branch = nullptr)
        : condition(condition), then_branch(then_branch), else_branch(else_branch) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitIfStmt(*this);
    }
    
    ExprPtr getCondition() const { return condition; }
    StmtPtr getThenBranch() const { return then_branch; }
    StmtPtr getElseBranch() const { return else_branch; }
    
private:
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
};

/**
 * @brief Represents a while statement
 */
class WhileStmt : public Statement {
public:
    WhileStmt(ExprPtr condition, StmtPtr body)
        : condition(condition), body(body) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitWhileStmt(*this);
    }
    
    ExprPtr getCondition() const { return condition; }
    StmtPtr getBody() const { return body; }
    
private:
    ExprPtr condition;
    StmtPtr body;
};

/**
 * @brief Represents a function declaration
 */
class FunctionStmt : public Statement {
public:
    FunctionStmt(Token name, std::vector<Token> params, std::vector<StmtPtr> body)
        : name(name), params(params), body(body) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitFunctionStmt(*this);
    }
    
    const Token& getName() const { return name; }
    const std::vector<Token>& getParams() const { return params; }
    const std::vector<StmtPtr>& getBody() const { return body; }
    
private:
    Token name;
    std::vector<Token> params;
    std::vector<StmtPtr> body;
};

/**
 * @brief Represents a return statement
 */
class ReturnStmt : public Statement {
public:
    ReturnStmt(Token keyword, ExprPtr value = nullptr)
        : keyword(keyword), value(value) {}
    
    void accept(AstVisitor& visitor) override {
        visitor.visitReturnStmt(*this);
    }
    
    const Token& getKeyword() const { return keyword; }
    ExprPtr getValue() const { return value; }
    
private:
    Token keyword;  // 'return' token, used for error reporting
    ExprPtr value;
};

} // namespace mana

#endif // MANASCRIPT_AST_HPP
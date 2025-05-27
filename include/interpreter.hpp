#ifndef MANASCRIPT_INTERPRETER_HPP
#define MANASCRIPT_INTERPRETER_HPP

#include "ast.hpp"
#include <unordered_map>
#include <string>
#include <variant>
#include <iostream>

namespace mana {

using Value = std::variant<int, double, std::string, bool, std::nullptr_t>;

class Interpreter : public AstVisitor {
public:
    Interpreter();
    void interpret(const std::vector<StmtPtr>& statements);

    // Expression visitors
    void visitLiteralExpr(LiteralExpr& expr) override;
    void visitUnaryExpr(UnaryExpr& expr) override;
    void visitBinaryExpr(BinaryExpr& expr) override;
    void visitGroupingExpr(GroupingExpr& expr) override;
    void visitVariableExpr(VariableExpr& expr) override;
    void visitAssignExpr(AssignExpr& expr) override;
    void visitCallExpr(CallExpr& expr) override;

    // Statement visitors
    void visitExpressionStmt(ExpressionStmt& stmt) override;
    void visitVarDeclStmt(VarDeclStmt& stmt) override;
    void visitBlockStmt(BlockStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;
    void visitFunctionStmt(FunctionStmt& stmt) override;
    void visitReturnStmt(ReturnStmt& stmt) override;

    Value getLastValue() const { return lastValue; }

private:
    Value evaluate(ExprPtr expr);
    void execute(StmtPtr stmt);
    std::unordered_map<std::string, Value> variables;
    Value lastValue;
};

} // namespace mana

#endif // MANASCRIPT_INTERPRETER_HPP 
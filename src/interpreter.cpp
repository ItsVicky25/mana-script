#include "interpreter.hpp"
#include <stdexcept>
#include <cmath>

namespace mana {

Interpreter::Interpreter() {}

void Interpreter::interpret(const std::vector<StmtPtr>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(*this);
    }
}

void Interpreter::execute(StmtPtr stmt) {
    stmt->accept(*this);
}

Value Interpreter::evaluate(ExprPtr expr) {
    expr->accept(*this);
    return lastValue;
}

// Expression visitors
void Interpreter::visitLiteralExpr(LiteralExpr& expr) {
    lastValue = expr.getValue();
}

void Interpreter::visitUnaryExpr(UnaryExpr& expr) {
    Value right = evaluate(expr.getRight());
    if (expr.getOperator().type == TokenType::MINUS) {
        if (std::holds_alternative<int>(right))
            lastValue = -std::get<int>(right);
        else if (std::holds_alternative<double>(right))
            lastValue = -std::get<double>(right);
        else
            throw std::runtime_error("Unary minus on non-number");
    } else if (expr.getOperator().type == TokenType::BANG) {
        if (std::holds_alternative<bool>(right))
            lastValue = !std::get<bool>(right);
        else
            throw std::runtime_error("Unary ! on non-bool");
    }
}

void Interpreter::visitBinaryExpr(BinaryExpr& expr) {
    Value left = evaluate(expr.getLeft());
    Value right = evaluate(expr.getRight());
    switch (expr.getOperator().type) {
        case TokenType::PLUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<int>(left) + std::get<int>(right);
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                lastValue = std::get<double>(left) + std::get<double>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                lastValue = std::get<std::string>(left) + std::get<std::string>(right);
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<std::string>(left) + std::to_string(std::get<int>(right));
            else if (std::holds_alternative<int>(left) && std::holds_alternative<std::string>(right))
                lastValue = std::to_string(std::get<int>(left)) + std::get<std::string>(right);
            else
                throw std::runtime_error("Invalid operands to +");
            break;
        case TokenType::MINUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<int>(left) - std::get<int>(right);
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                lastValue = std::get<double>(left) - std::get<double>(right);
            else
                throw std::runtime_error("Invalid operands to -");
            break;
        case TokenType::STAR:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<int>(left) * std::get<int>(right);
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                lastValue = std::get<double>(left) * std::get<double>(right);
            else
                throw std::runtime_error("Invalid operands to *");
            break;
        case TokenType::SLASH:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                if (std::get<int>(right) == 0) throw std::runtime_error("Division by zero");
                lastValue = std::get<int>(left) / std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                if (std::get<double>(right) == 0.0) throw std::runtime_error("Division by zero");
                lastValue = std::get<double>(left) / std::get<double>(right);
            } else
                throw std::runtime_error("Invalid operands to /");
            break;
        case TokenType::EQUAL_EQUAL:
            lastValue = left == right;
            break;
        case TokenType::BANG_EQUAL:
            lastValue = left != right;
            break;
        case TokenType::LESS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<int>(left) < std::get<int>(right);
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                lastValue = std::get<double>(left) < std::get<double>(right);
            else
                throw std::runtime_error("Invalid operands to <");
            break;
        case TokenType::LESS_EQUAL:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<int>(left) <= std::get<int>(right);
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                lastValue = std::get<double>(left) <= std::get<double>(right);
            else
                throw std::runtime_error("Invalid operands to <=");
            break;
        case TokenType::GREATER:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<int>(left) > std::get<int>(right);
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                lastValue = std::get<double>(left) > std::get<double>(right);
            else
                throw std::runtime_error("Invalid operands to >");
            break;
        case TokenType::GREATER_EQUAL:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                lastValue = std::get<int>(left) >= std::get<int>(right);
            else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
                lastValue = std::get<double>(left) >= std::get<double>(right);
            else
                throw std::runtime_error("Invalid operands to >=");
            break;
        default:
            throw std::runtime_error("Unknown binary operator");
    }
}

void Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    lastValue = evaluate(expr.getExpression());
}

void Interpreter::visitVariableExpr(VariableExpr& expr) {
    auto it = variables.find(expr.getName().lexeme);
    if (it == variables.end()) throw std::runtime_error("Undefined variable: " + expr.getName().lexeme);
    lastValue = it->second;
}

void Interpreter::visitAssignExpr(AssignExpr& expr) {
    Value value = evaluate(expr.getValue());
    variables[expr.getName().lexeme] = value;
    lastValue = value;
}

void Interpreter::visitCallExpr(CallExpr& expr) {
    // Only support print for now
    if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr.getCallee())) {
        if (var->getName().lexeme == "print") {
            for (const auto& arg : expr.getArguments()) {
                Value v = evaluate(arg);
                if (std::holds_alternative<int>(v))
                    std::cout << std::get<int>(v) << std::endl;
                else if (std::holds_alternative<double>(v))
                    std::cout << std::get<double>(v) << std::endl;
                else if (std::holds_alternative<std::string>(v))
                    std::cout << std::get<std::string>(v) << std::endl;
                else if (std::holds_alternative<bool>(v))
                    std::cout << (std::get<bool>(v) ? "true" : "false") << std::endl;
                else if (std::holds_alternative<std::nullptr_t>(v))
                    std::cout << "nil" << std::endl;
            }
            lastValue = nullptr;
            return;
        }
    }
    throw std::runtime_error("Only print() is supported as a built-in function");
}

// Statement visitors
void Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    lastValue = evaluate(stmt.getExpression());
}

void Interpreter::visitVarDeclStmt(VarDeclStmt& stmt) {
    Value value = nullptr;
    if (stmt.getInitializer()) {
        value = evaluate(stmt.getInitializer());
    }
    variables[stmt.getName().lexeme] = value;
    lastValue = value;
}

void Interpreter::visitBlockStmt(BlockStmt& stmt) {
    for (const auto& s : stmt.getStatements()) {
        execute(s);
    }
}

void Interpreter::visitIfStmt(IfStmt& stmt) {
    Value cond = evaluate(stmt.getCondition());
    bool condVal = false;
    if (std::holds_alternative<bool>(cond))
        condVal = std::get<bool>(cond);
    else if (std::holds_alternative<int>(cond))
        condVal = std::get<int>(cond) != 0;
    else
        throw std::runtime_error("Invalid condition in if statement");
    if (condVal) {
        execute(stmt.getThenBranch());
    } else if (stmt.getElseBranch()) {
        execute(stmt.getElseBranch());
    }
}

void Interpreter::visitWhileStmt(WhileStmt&) {
    // Not implemented
}

void Interpreter::visitFunctionStmt(FunctionStmt&) {
    // Not implemented
}

void Interpreter::visitReturnStmt(ReturnStmt&) {
    // Not implemented
}

} // namespace mana 
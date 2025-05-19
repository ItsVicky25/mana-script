#include "transpiler.hpp"

namespace mana {

Transpiler::Transpiler() {
    // Initialize type mapping
    type_map["int"] = "int";
    type_map["float"] = "double";
    type_map["bool"] = "bool";
    type_map["string"] = "std::string";
    type_map["void"] = "void";
}

void Transpiler::indent() {
    for (int i = 0; i < indent_level; ++i) {
        output << "    ";
    }
}

void Transpiler::writeLine(const std::string& line) {
    indent();
    output << line << "\n";
}

void Transpiler::write(const std::string& text) {
    output << text;
}

std::string Transpiler::getTypeName(const std::string& mana_type) {
    auto it = type_map.find(mana_type);
    if (it != type_map.end()) {
        return it->second;
    }
    return "auto"; // Default to auto if type is unknown
}

std::string Transpiler::transpile(const std::vector<StmtPtr>& statements) {
    // Add includes and namespace
    output.str("");
    output << "#include <iostream>\n";
    output << "#include <string>\n";
    output << "#include <vector>\n";
    output << "#include <functional>\n";
    output << "#include <cmath>\n\n";
    
    // Add any helper functions or runtime support
    output << "// Manascript runtime support\n";
    output << "void print(const std::string& message) {\n";
    output << "    std::cout << message << std::endl;\n";
    output << "}\n\n";
    
    // Transpile statements
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
    
    // If no main function was defined, create a simple one
    if (output.str().find("int main(") == std::string::npos) {
        output << "\n// Auto-generated main function\n";
        output << "int main() {\n";
        output << "    // Your code here\n";
        output << "    return 0;\n";
        output << "}\n";
    }
    
    return output.str();
}

// Expression visitors
void Transpiler::visitLiteralExpr(LiteralExpr& expr) {
    const auto& value = expr.getValue();
    
    if (std::holds_alternative<int>(value)) {
        write(std::to_string(std::get<int>(value)));
    }
    else if (std::holds_alternative<double>(value)) {
        write(std::to_string(std::get<double>(value)));
    }
    else if (std::holds_alternative<bool>(value)) {
        write(std::get<bool>(value) ? "true" : "false");
    }
    else if (std::holds_alternative<std::string>(value)) {
        write("\"" + std::get<std::string>(value) + "\"");
    }
    else if (std::holds_alternative<std::nullptr_t>(value)) {
        write("nullptr");
    }
}

void Transpiler::visitUnaryExpr(UnaryExpr& expr) {
    TokenType op = expr.getOperator().type;
    
    if (op == TokenType::MINUS) {
        write("-");
    }
    else if (op == TokenType::BANG) {
        write("!");
    }
    
    write("(");
    expr.getRight()->accept(*this);
    write(")");
}

void Transpiler::visitBinaryExpr(BinaryExpr& expr) {
    write("(");
    expr.getLeft()->accept(*this);
    
    TokenType op = expr.getOperator().type;
    switch (op) {
        case TokenType::PLUS:          write(" + "); break;
        case TokenType::MINUS:         write(" - "); break;
        case TokenType::STAR:          write(" * "); break;
        case TokenType::SLASH:         write(" / "); break;
        case TokenType::PERCENT:       write(" % "); break;
        case TokenType::EQUAL_EQUAL:   write(" == "); break;
        case TokenType::BANG_EQUAL:    write(" != "); break;
        case TokenType::LESS:          write(" < "); break;
        case TokenType::LESS_EQUAL:    write(" <= "); break;
        case TokenType::GREATER:       write(" > "); break;
        case TokenType::GREATER_EQUAL: write(" >= "); break;
        case TokenType::AND:           write(" && "); break;
        case TokenType::OR:            write(" || "); break;
        default: write(" ? "); break;
    }
    
    expr.getRight()->accept(*this);
    write(")");
}

void Transpiler::visitGroupingExpr(GroupingExpr& expr) {
    write("(");
    expr.getExpression()->accept(*this);
    write(")");
}

void Transpiler::visitVariableExpr(VariableExpr& expr) {
    write(expr.getName().lexeme);
}

void Transpiler::visitAssignExpr(AssignExpr& expr) {
    write(expr.getName().lexeme + " = ");
    expr.getValue()->accept(*this);
}

void Transpiler::visitCallExpr(CallExpr& expr) {
    expr.getCallee()->accept(*this);
    write("(");
    
    const auto& args = expr.getArguments();
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            write(", ");
        }
        args[i]->accept(*this);
    }
    
    write(")");
}

// Statement visitors
void Transpiler::visitExpressionStmt(ExpressionStmt& stmt) {
    indent();
    stmt.getExpression()->accept(*this);
    write(";\n");
}

void Transpiler::visitVarDeclStmt(VarDeclStmt& stmt) {
    indent();
    
    // Determine type or use auto
    write(stmt.isConst() ? "const auto " : "auto ");
    
    write(stmt.getName().lexeme);
    
    if (stmt.getInitializer()) {
        write(" = ");
        stmt.getInitializer()->accept(*this);
    }
    
    write(";\n");
}

void Transpiler::visitBlockStmt(BlockStmt& stmt) {
    writeLine("{");
    
    indent_level++;
    for (const auto& s : stmt.getStatements()) {
        if (s) {
            s->accept(*this);
        }
    }
    indent_level--;
    
    writeLine("}");
}

void Transpiler::visitIfStmt(IfStmt& stmt) {
    indent();
    write("if (");
    stmt.getCondition()->accept(*this);
    write(") ");
    
    if (auto* block = dynamic_cast<BlockStmt*>(stmt.getThenBranch().get())) {
        block->accept(*this);
    } else {
        writeLine("{");
        indent_level++;
        stmt.getThenBranch()->accept(*this);
        indent_level--;
        writeLine("}");
    }
    
    if (stmt.getElseBranch()) {
        indent();
        write("else ");
        
        if (auto* block = dynamic_cast<BlockStmt*>(stmt.getElseBranch().get())) {
            block->accept(*this);
        } else {
            writeLine("{");
            indent_level++;
            stmt.getElseBranch()->accept(*this);
            indent_level--;
            writeLine("}");
        }
    }
}

void Transpiler::visitWhileStmt(WhileStmt& stmt) {
    indent();
    write("while (");
    stmt.getCondition()->accept(*this);
    write(") ");
    
    if (auto* block = dynamic_cast<BlockStmt*>(stmt.getBody().get())) {
        block->accept(*this);
    } else {
        writeLine("{");
        indent_level++;
        stmt.getBody()->accept(*this);
        indent_level--;
        writeLine("}");
    }
}

void Transpiler::visitFunctionStmt(FunctionStmt& stmt) {
    // Determine if this is the main function
    bool is_main = stmt.getName().lexeme == "main";
    
    indent();
    
    // Write return type
    if (is_main) {
        write("int ");
    } else {
        write("auto ");
    }
    
    // Write function name and parameters
    write(stmt.getName().lexeme + "(");
    
    const auto& params = stmt.getParams();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) {
            write(", ");
        }
        write("auto " + params[i].lexeme);
    }
    
    write(") ");
    
    // Write function body
    writeLine("{");
    indent_level++;
    
    for (const auto& s : stmt.getBody()) {
        if (s) {
            s->accept(*this);
        }
    }
    
    // Add default return for main
    if (is_main) {
        writeLine("return 0;");
    }
    
    indent_level--;
    writeLine("}");
}

void Transpiler::visitReturnStmt(ReturnStmt& stmt) {
    indent();
    write("return");
    
    if (stmt.getValue()) {
        write(" ");
        stmt.getValue()->accept(*this);
    }
    
    write(";\n");
}

} // namespace mana
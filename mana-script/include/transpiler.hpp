#ifndef MANASCRIPT_TRANSPILER_HPP
#define MANASCRIPT_TRANSPILER_HPP

#include "ast.hpp"
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace mana {

/**
 * @brief Transpiles Manascript AST to C++ code
 */
class Transpiler : public AstVisitor {
private:
    std::stringstream output;
    int indent_level = 0;
    std::unordered_map<std::string, std::string> type_map;
    std::vector<std::string> current_var_decls;
    
    // Helper methods
    void indent();
    void writeLine(const std::string& line);
    void write(const std::string& text);
    std::string getTypeName(const std::string& mana_type);
    
public:
    Transpiler();
    
    /**
     * @brief Transpile AST to C++ code
     * @param statements AST statements to transpile
     * @return Generated C++ code
     */
    std::string transpile(const std::vector<StmtPtr>& statements);
    
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
};

} // namespace mana

#endif // MANASCRIPT_TRANSPILER_HPP
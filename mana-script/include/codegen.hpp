#ifndef MANASCRIPT_CODEGEN_HPP
#define MANASCRIPT_CODEGEN_HPP

#include "ast.hpp"
#include "symbol_table.hpp"
#include "error.hpp"

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace mana {

/**
 * @brief Generates LLVM IR from the AST
 */
class CodeGenerator : public AstVisitor {
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    
    SymbolTable symbol_table;
    
    // Value stack for expression evaluation
    std::vector<llvm::Value*> value_stack;
    
    // Function and variable mapping
    std::unordered_map<std::string, llvm::Function*> functions;
    std::unordered_map<std::string, llvm::AllocaInst*> named_values;
    
    // Current function being compiled
    llvm::Function* current_function = nullptr;
    
    // Helper methods
    llvm::Type* getIntType();
    llvm::Type* getFloatType();
    llvm::Type* getBoolType();
    llvm::Type* getVoidType();
    llvm::Type* getStringType();
    
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function, 
                                            const std::string& name,
                                            llvm::Type* type);
    
    // Push and pop values from the value stack
    void pushValue(llvm::Value* value);
    llvm::Value* popValue();
    llvm::Value* getCurrentValue();
    
    // Create basic library functions
    void createPrintFunction();
    
public:
    CodeGenerator();
    
    // Initialize code generation
    void initialize(const std::string& module_name);
    
    // Generate code for a program
    void generate(const std::vector<StmtPtr>& statements);
    
    // Get the generated LLVM module
    llvm::Module* getModule() { return module.get(); }
    
    // Dump LLVM IR to a string
    std::string dumpIR() const;
    
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

#endif // MANASCRIPT_CODEGEN_HPP
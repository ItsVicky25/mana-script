#include "codegen.hpp"
#include <iostream>
#include <sstream>
#include <vector>

namespace mana {

CodeGenerator::CodeGenerator() {}

void CodeGenerator::initialize(const std::string& module_name) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(module_name, *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    
    // Initialize built-in functions
    createPrintFunction();
}

void CodeGenerator::generate(const std::vector<StmtPtr>& statements) {
    // Create main function
    llvm::FunctionType* main_type = llvm::FunctionType::get(
        getIntType(), false
    );
    
    llvm::Function* main_func = llvm::Function::Create(
        main_type, llvm::Function::ExternalLinkage, "main", module.get()
    );
    
    // Create entry block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", main_func);
    builder->SetInsertPoint(entry);
    
    // Set current function
    current_function = main_func;
    
    // Generate code for statements
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
    
    // Return 0 from main
    builder->CreateRet(llvm::ConstantInt::get(getIntType(), 0));
    
    // Verify the module
    std::string error_info;
    llvm::raw_string_ostream error_stream(error_info);
    if (llvm::verifyModule(*module, &error_stream)) {
        diagnostics.report(
            DiagnosticSeverity::ERROR,
            "LLVM IR verification failed: " + error_stream.str(),
            SourceLocation()
        );
    }
}

void CodeGenerator::createPrintFunction() {
    // Declare printf function
    std::vector<llvm::Type*> printf_args;
    printf_args.push_back(llvm::Type::getInt8PtrTy(*context));
    llvm::FunctionType* printf_type = llvm::FunctionType::get(
        getIntType(), printf_args, true
    );
    llvm::Function::Create(
        printf_type, llvm::Function::ExternalLinkage, "printf", module.get()
    );
    
    // Create print function that wraps printf
    std::vector<llvm::Type*> print_args;
    print_args.push_back(llvm::Type::getInt8PtrTy(*context));
    llvm::FunctionType* print_type = llvm::FunctionType::get(
        getVoidType(), print_args, false
    );
    llvm::Function* print_func = llvm::Function::Create(
        print_type, llvm::Function::ExternalLinkage, "print", module.get()
    );
    
    // Set argument name
    print_func->arg_begin()->setName("format");
    
    // Create basic block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", print_func);
    builder->SetInsertPoint(entry);
    
    // Get printf function
    llvm::Function* printf_func = module->getFunction("printf");
    
    // Call printf with the format string
    std::vector<llvm::Value*> args;
    args.push_back(print_func->arg_begin());
    builder->CreateCall(printf_func, args);
    
    // Return from print
    builder->CreateRetVoid();
    
    // Add to function map
    functions["print"] = print_func;
}

llvm::Type* CodeGenerator::getIntType() {
    return llvm::Type::getInt32Ty(*context);
}

llvm::Type* CodeGenerator::getFloatType() {
    return llvm::Type::getDoubleTy(*context);
}

llvm::Type* CodeGenerator::getBoolType() {
    return llvm::Type::getInt1Ty(*context);
}

llvm::Type* CodeGenerator::getVoidType() {
    return llvm::Type::getVoidTy(*context);
}

llvm::Type* CodeGenerator::getStringType() {
    return llvm::Type::getInt8PtrTy(*context);
}

llvm::AllocaInst* CodeGenerator::createEntryBlockAlloca(
    llvm::Function* function, const std::string& name, llvm::Type* type) {
    
    llvm::IRBuilder<> temp_builder(
        &function->getEntryBlock(),
        function->getEntryBlock().begin()
    );
    
    return temp_builder.CreateAlloca(type, nullptr, name);
}

void CodeGenerator::pushValue(llvm::Value* value) {
    value_stack.push_back(value);
}

llvm::Value* CodeGenerator::popValue() {
    if (value_stack.empty()) {
        return nullptr;
    }
    
    llvm::Value* value = value_stack.back();
    value_stack.pop_back();
    return value;
}

llvm::Value* CodeGenerator::getCurrentValue() {
    if (value_stack.empty()) {
        return nullptr;
    }
    
    return value_stack.back();
}

std::string CodeGenerator::dumpIR() const {
    std::string ir;
    llvm::raw_string_ostream os(ir);
    module->print(os, nullptr);
    return os.str();
}

// Expression visitors
void CodeGenerator::visitLiteralExpr(LiteralExpr& expr) {
    const auto& value = expr.getValue();
    
    if (std::holds_alternative<int>(value)) {
        pushValue(llvm::ConstantInt::get(getIntType(), std::get<int>(value)));
    }
    else if (std::holds_alternative<double>(value)) {
        pushValue(llvm::ConstantFP::get(getFloatType(), std::get<double>(value)));
    }
    else if (std::holds_alternative<bool>(value)) {
        pushValue(llvm::ConstantInt::get(getBoolType(), std::get<bool>(value)));
    }
    else if (std::holds_alternative<std::string>(value)) {
        // Create a global string constant
        llvm::Constant* str_const = llvm::ConstantDataArray::getString(
            *context, std::get<std::string>(value)
        );
        
        llvm::GlobalVariable* global_str = new llvm::GlobalVariable(
            *module, str_const->getType(), true,
            llvm::GlobalValue::PrivateLinkage, str_const, ".str"
        );
        
        // Get a pointer to the start of the string
        std::vector<llvm::Value*> indices = {
            llvm::ConstantInt::get(getIntType(), 0),
            llvm::ConstantInt::get(getIntType(), 0)
        };
        
        pushValue(builder->CreateInBoundsGEP(
            global_str->getValueType(), global_str, indices, "str_ptr"
        ));
    }
    else if (std::holds_alternative<std::nullptr_t>(value)) {
        pushValue(llvm::ConstantPointerNull::get(
            llvm::Type::getInt8PtrTy(*context)
        ));
    }
}

void CodeGenerator::visitUnaryExpr(UnaryExpr& expr) {
    expr.getRight()->accept(*this);
    llvm::Value* operand = popValue();
    
    if (!operand) {
        diagnostics.report(
            DiagnosticSeverity::ERROR,
            "Invalid operand for unary operator",
            SourceLocation()
        );
        pushValue(nullptr);
        return;
    }
    
    TokenType op = expr.getOperator().type;
    
    if (op == TokenType::MINUS) {
        if (operand->getType()->isIntegerTy()) {
            pushValue(builder->CreateNeg(operand, "neg"));
        }
        else if (operand->getType()->isFloatingPointTy()) {
            pushValue(builder->CreateFNeg(operand, "fneg"));
        }
        else {
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Invalid operand type for unary minus",
                SourceLocation()
            );
            pushValue(nullptr);
        }
    }
    else if (op == TokenType::BANG) {
        if (operand->getType()->isIntegerTy()) {
            // Convert any integer to bool first (0 = false, non-0 = true)
            llvm::Value* bool_val = builder->CreateICmpNE(
                operand, llvm::ConstantInt::get(operand->getType(), 0), "tobool"
            );
            pushValue(builder->CreateNot(bool_val, "not"));
        }
        else {
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Invalid operand type for logical not",
                SourceLocation()
            );
            pushValue(nullptr);
        }
    }
}

void CodeGenerator::visitBinaryExpr(BinaryExpr& expr) {
    // Special case for logical AND/OR (short-circuit evaluation)
    if (expr.getOperator().type == TokenType::AND ||
        expr.getOperator().type == TokenType::OR) {
        
        // Create basic blocks for short-circuit evaluation
        llvm::Function* function = builder->GetInsertBlock()->getParent();
        llvm::BasicBlock* right_bb = llvm::BasicBlock::Create(*context, "right", function);
        llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*context, "merge", function);
        
        // Evaluate left operand
        expr.getLeft()->accept(*this);
        llvm::Value* left = popValue();
        
        if (!left || !left->getType()->isIntegerTy()) {
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Left operand of logical operator must be a boolean",
                SourceLocation()
            );
            pushValue(nullptr);
            return;
        }
        
        // Convert to boolean if needed
        if (!left->getType()->isIntegerTy(1)) {
            left = builder->CreateICmpNE(
                left, llvm::ConstantInt::get(left->getType(), 0), "tobool"
            );
        }
        
        // Short-circuit based on the operator
        if (expr.getOperator().type == TokenType::AND) {
            // AND: if left is false, skip right and use false
            builder->CreateCondBr(left, right_bb, merge_bb);
        } else {
            // OR: if left is true, skip right and use true
            builder->CreateCondBr(left, merge_bb, right_bb);
        }
        
        // Evaluate right operand in right_bb
        builder->SetInsertPoint(right_bb);
        expr.getRight()->accept(*this);
        llvm::Value* right = popValue();
        
        if (!right || !right->getType()->isIntegerTy()) {
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Right operand of logical operator must be a boolean",
                SourceLocation()
            );
            pushValue(nullptr);
            return;
        }
        
        // Convert to boolean if needed
        if (!right->getType()->isIntegerTy(1)) {
            right = builder->CreateICmpNE(
                right, llvm::ConstantInt::get(right->getType(), 0), "tobool"
            );
        }
        
        builder->CreateBr(merge_bb);
        right_bb = builder->GetInsertBlock(); // Update in case of codegen in right side
        
        // Set up phi node in merge block
        builder->SetInsertPoint(merge_bb);
        llvm::PHINode* phi = builder->CreatePHI(getBoolType(), 2, "logical");
        
        if (expr.getOperator().type == TokenType::AND) {
            phi->addIncoming(llvm::ConstantInt::getFalse(*context), builder->GetInsertBlock()->getParent()->getEntryBlock().getTerminator()->getParent());
            phi->addIncoming(right, right_bb);
        } else {
            phi->addIncoming(llvm::ConstantInt::getTrue(*context), builder->GetInsertBlock()->getParent()->getEntryBlock().getTerminator()->getParent());
            phi->addIncoming(right, right_bb);
        }
        
        pushValue(phi);
        return;
    }
    
    // Regular binary operators
    expr.getLeft()->accept(*this);
    llvm::Value* left = popValue();
    
    expr.getRight()->accept(*this);
    llvm::Value* right = popValue();
    
    if (!left || !right) {
        diagnostics.report(
            DiagnosticSeverity::ERROR,
            "Invalid operands for binary operation",
            SourceLocation()
        );
        pushValue(nullptr);
        return;
    }
    
    TokenType op = expr.getOperator().type;
    bool is_integer_op = left->getType()->isIntegerTy() && right->getType()->isIntegerTy();
    bool is_float_op = left->getType()->isFloatingPointTy() || right->getType()->isFloatingPointTy();
    
    // Promote ints to floats if mixed types
    if (is_float_op && left->getType()->isIntegerTy()) {
        left = builder->CreateSIToFP(left, getFloatType(), "int2float");
    }
    else if (is_float_op && right->getType()->isIntegerTy()) {
        right = builder->CreateSIToFP(right, getFloatType(), "int2float");
    }
    
    // Generate code based on operator type
    switch (op) {
        case TokenType::PLUS:
            if (is_float_op) {
                pushValue(builder->CreateFAdd(left, right, "fadd"));
            } else if (is_integer_op) {
                pushValue(builder->CreateAdd(left, right, "add"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for addition",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::MINUS:
            if (is_float_op) {
                pushValue(builder->CreateFSub(left, right, "fsub"));
            } else if (is_integer_op) {
                pushValue(builder->CreateSub(left, right, "sub"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for subtraction",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::STAR:
            if (is_float_op) {
                pushValue(builder->CreateFMul(left, right, "fmul"));
            } else if (is_integer_op) {
                pushValue(builder->CreateMul(left, right, "mul"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for multiplication",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::SLASH:
            if (is_float_op) {
                pushValue(builder->CreateFDiv(left, right, "fdiv"));
            } else if (is_integer_op) {
                pushValue(builder->CreateSDiv(left, right, "div"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for division",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::PERCENT:
            if (is_integer_op) {
                pushValue(builder->CreateSRem(left, right, "rem"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Modulo operator requires integer operands",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        // Comparison operators
        case TokenType::EQUAL_EQUAL:
            if (is_float_op) {
                pushValue(builder->CreateFCmpOEQ(left, right, "feq"));
            } else if (is_integer_op) {
                pushValue(builder->CreateICmpEQ(left, right, "eq"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for equality comparison",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::BANG_EQUAL:
            if (is_float_op) {
                pushValue(builder->CreateFCmpONE(left, right, "fne"));
            } else if (is_integer_op) {
                pushValue(builder->CreateICmpNE(left, right, "ne"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for inequality comparison",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::LESS:
            if (is_float_op) {
                pushValue(builder->CreateFCmpOLT(left, right, "flt"));
            } else if (is_integer_op) {
                pushValue(builder->CreateICmpSLT(left, right, "lt"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for less-than comparison",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::LESS_EQUAL:
            if (is_float_op) {
                pushValue(builder->CreateFCmpOLE(left, right, "fle"));
            } else if (is_integer_op) {
                pushValue(builder->CreateICmpSLE(left, right, "le"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for less-than-or-equal comparison",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::GREATER:
            if (is_float_op) {
                pushValue(builder->CreateFCmpOGT(left, right, "fgt"));
            } else if (is_integer_op) {
                pushValue(builder->CreateICmpSGT(left, right, "gt"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for greater-than comparison",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        case TokenType::GREATER_EQUAL:
            if (is_float_op) {
                pushValue(builder->CreateFCmpOGE(left, right, "fge"));
            } else if (is_integer_op) {
                pushValue(builder->CreateICmpSGE(left, right, "ge"));
            } else {
                diagnostics.report(
                    DiagnosticSeverity::ERROR,
                    "Invalid operands for greater-than-or-equal comparison",
                    SourceLocation()
                );
                pushValue(nullptr);
            }
            break;
            
        default:
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Unknown binary operator",
                SourceLocation()
            );
            pushValue(nullptr);
            break;
    }
}

void CodeGenerator::visitGroupingExpr(GroupingExpr& expr) {
    expr.getExpression()->accept(*this);
    // Value is already on the stack
}

void CodeGenerator::visitVariableExpr(VariableExpr& expr) {
    std::string name = expr.getName().lexeme;
    auto it = named_values.find(name);
    
    if (it == named_values.end()) {
        diagnostics.report(
            DiagnosticSeverity::ERROR,
            "Unknown variable name: " + name,
            SourceLocation()
        );
        pushValue(nullptr);
        return;
    }
    
    llvm::Value* value = builder->CreateLoad(
        it->second->getAllocatedType(), it->second, name
    );
    pushValue(value);
}

void CodeGenerator::visitAssignExpr(AssignExpr& expr) {
    expr.getValue()->accept(*this);
    llvm::Value* value = popValue();
    
    std::string name = expr.getName().lexeme;
    auto it = named_values.find(name);
    
    if (it == named_values.end()) {
        diagnostics.report(
            DiagnosticSeverity::ERROR,
            "Unknown variable name: " + name,
            SourceLocation()
        );
        pushValue(nullptr);
        return;
    }
    
    builder->CreateStore(value, it->second);
    pushValue(value);
}

void CodeGenerator::visitCallExpr(CallExpr& expr) {
    llvm::Function* callee = nullptr;
    
    // Handle direct function calls
    if (auto* var_expr = dynamic_cast<VariableExpr*>(expr.getCallee().get())) {
        std::string func_name = var_expr->getName().lexeme;
        callee = module->getFunction(func_name);
        
        if (!callee) {
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Unknown function name: " + func_name,
                SourceLocation()
            );
            pushValue(nullptr);
            return;
        }
    }
    else {
        // Handle function pointers
        expr.getCallee()->accept(*this);
        llvm::Value* callee_val = popValue();
        
        if (!callee_val || !callee_val->getType()->isPointerTy()) {
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Expression is not callable",
                SourceLocation()
            );
            pushValue(nullptr);
            return;
        }
        
        // Try to cast to a function pointer
        llvm::PointerType* ptr_type = llvm::dyn_cast<llvm::PointerType>(callee_val->getType());
        if (ptr_type->getElementType()->isFunctionTy()) {
            // Cast to an appropriate function type
            callee_val = builder->CreateBitCast(
                callee_val, 
                llvm::PointerType::get(llvm::dyn_cast<llvm::FunctionType>(ptr_type->getElementType()), 0),
                "callee"
            );
        } else {
            diagnostics.report(
                DiagnosticSeverity::ERROR,
                "Expression is not callable",
                SourceLocation()
            );
            pushValue(nullptr);
            return;
        }
    }
    
    // Evaluate arguments
    std::vector<llvm::Value*> args;
    for (const auto& arg : expr.getArguments()) {
        arg->accept(*this);
        args.push_back(popValue());
    }
    
    // Create call
    llvm::Value* call = builder->CreateCall(callee, args, "call");
    pushValue(call);
}

// Statement visitors
void CodeGenerator::visitExpressionStmt(ExpressionStmt& stmt) {
    stmt.getExpression()->accept(*this);
    popValue(); // Discard the result
}

void CodeGenerator::visitVarDeclStmt(VarDeclStmt& stmt) {
    std::string name = stmt.getName().lexeme;
    
    // Determine type (default to int)
    llvm::Type* var_type = getIntType();
    
    // Evaluate initializer if present
    llvm::Value* init_val = nullptr;
    if (stmt.getInitializer()) {
        stmt.getInitializer()->accept(*this);
        init_val = popValue();
        
        if (init_val) {
            var_type = init_val->getType();
        }
    }
    
    // Create variable in current scope
    llvm::AllocaInst* alloca = createEntryBlockAlloca(
        current_function, name, var_type
    );
    
    // Store initial value if present
    if (init_val) {
        builder->CreateStore(init_val, alloca);
    }
    
    // Add to symbol table
    named_values[name] = alloca;
    symbol_table.define(name, Symbol::Kind::VARIABLE);
}

void CodeGenerator::visitBlockStmt(BlockStmt& stmt) {
    // Create a new scope
    symbol_table.enterScope();
    
    // Process statements in the block
    for (const auto& s : stmt.getStatements()) {
        if (s) {
            s->accept(*this);
        }
    }
    
    // Exit the scope
    symbol_table.exitScope();
}

void CodeGenerator::visitIfStmt(IfStmt& stmt) {
    // Evaluate condition
    stmt.getCondition()->accept(*this);
    llvm::Value* cond_val = popValue();
    
    if (!cond_val) {
        return;
    }
    
    // Convert to boolean if needed
    if (!cond_val->getType()->isIntegerTy(1)) {
        cond_val = builder->CreateICmpNE(
            cond_val, llvm::ConstantInt::get(cond_val->getType(), 0), "ifcond"
        );
    }
    
    // Create basic blocks for the then, else, and merge points
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* then_bb = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* else_bb = llvm::BasicBlock::Create(*context, "else");
    llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*context, "ifcont");
    
    // Create conditional branch
    builder->CreateCondBr(cond_val, then_bb, else_bb);
    
    // Emit then block
    builder->SetInsertPoint(then_bb);
    stmt.getThenBranch()->accept(*this);
    builder->CreateBr(merge_bb);
    
    // Get the updated then block in case of nested blocks
    then_bb = builder->GetInsertBlock();
    
    // Emit else block
    function->getBasicBlockList().push_back(else_bb);
    builder->SetInsertPoint(else_bb);
    
    if (stmt.getElseBranch()) {
        stmt.getElseBranch()->accept(*this);
    }
    
    builder->CreateBr(merge_bb);
    
    // Get the updated else block in case of nested blocks
    else_bb = builder->GetInsertBlock();
    
    // Emit merge block
    function->getBasicBlockList().push_back(merge_bb);
    builder->SetInsertPoint(merge_bb);
}

void CodeGenerator::visitWhileStmt(WhileStmt& stmt) {
    // Create basic blocks for the condition, loop body, and exit points
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* cond_bb = llvm::BasicBlock::Create(*context, "while.cond", function);
    llvm::BasicBlock* body_bb = llvm::BasicBlock::Create(*context, "while.body");
    llvm::BasicBlock* exit_bb = llvm::BasicBlock::Create(*context, "while.exit");
    
    // Branch to condition
    builder->CreateBr(cond_bb);
    
    // Emit condition block
    builder->SetInsertPoint(cond_bb);
    stmt.getCondition()->accept(*this);
    llvm::Value* cond_val = popValue();
    
    if (!cond_val) {
        return;
    }
    
    // Convert to boolean if needed
    if (!cond_val->getType()->isIntegerTy(1)) {
        cond_val = builder->CreateICmpNE(
            cond_val, llvm::ConstantInt::get(cond_val->getType(), 0), "whilecond"
        );
    }
    
    // Create conditional branch
    builder->CreateCondBr(cond_val, body_bb, exit_bb);
    
    // Emit body block
    function->getBasicBlockList().push_back(body_bb);
    builder->SetInsertPoint(body_bb);
    
    stmt.getBody()->accept(*this);
    
    // Branch back to condition
    builder->CreateBr(cond_bb);
    
    // Emit exit block
    function->getBasicBlockList().push_back(exit_bb);
    builder->SetInsertPoint(exit_bb);
}

void CodeGenerator::visitFunctionStmt(FunctionStmt& stmt) {
    std::string name = stmt.getName().lexeme;
    
    // Create function type
    std::vector<llvm::Type*> param_types(stmt.getParams().size(), getIntType());
    llvm::Type* return_type = getIntType(); // Default to int return type
    
    llvm::FunctionType* func_type = llvm::FunctionType::get(
        return_type, param_types, false
    );
    
    // Create function
    llvm::Function* function = llvm::Function::Create(
        func_type, llvm::Function::ExternalLinkage, name, module.get()
    );
    
    // Set parameter names
    unsigned idx = 0;
    for (auto& arg : function->args()) {
        arg.setName(stmt.getParams()[idx++].lexeme);
    }
    
    // Add to functions map
    functions[name] = function;
    
    // Create a new basic block for the function body
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(entry);
    
    // Store the previous function and set the current one
    llvm::Function* prev_function = current_function;
    current_function = function;
    
    // Create a new scope for the function
    symbol_table.enterScope();
    
    // Create allocas for parameters and add to symbol table
    for (auto& arg : function->args()) {
        llvm::AllocaInst* alloca = createEntryBlockAlloca(
            function, arg.getName().str(), arg.getType()
        );
        
        builder->CreateStore(&arg, alloca);
        named_values[arg.getName().str()] = alloca;
        symbol_table.define(arg.getName().str(), Symbol::Kind::PARAMETER);
    }
    
    // Generate code for function body
    for (const auto& s : stmt.getBody()) {
        if (s) {
            s->accept(*this);
        }
    }
    
    // Add a default return if there isn't one already
    if (builder->GetInsertBlock()->getTerminator() == nullptr) {
        // Default return 0 for int functions
        builder->CreateRet(llvm::ConstantInt::get(getIntType(), 0));
    }
    
    // Exit the function scope
    symbol_table.exitScope();
    
    // Restore the previous function
    current_function = prev_function;
    
    // Verify the function
    if (llvm::verifyFunction(*function, &llvm::errs())) {
        // Function verification failed
        function->eraseFromParent();
        functions.erase(name);
        
        diagnostics.report(
            DiagnosticSeverity::ERROR,
            "Function verification failed: " + name,
            SourceLocation()
        );
    }
}

void CodeGenerator::visitReturnStmt(ReturnStmt& stmt) {
    if (!current_function) {
        diagnostics.report(
            DiagnosticSeverity::ERROR,
            "Return statement outside of function",
            SourceLocation()
        );
        return;
    }
    
    llvm::Value* return_val = nullptr;
    
    if (stmt.getValue()) {
        stmt.getValue()->accept(*this);
        return_val = popValue();
    } else {
        // Default return value is 0 for int functions
        return_val = llvm::ConstantInt::get(getIntType(), 0);
    }
    
    if (return_val) {
        builder->CreateRet(return_val);
    } else {
        builder->CreateRetVoid();
    }
}

} // namespace mana
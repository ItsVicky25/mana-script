#ifndef MANASCRIPT_JIT_HPP
#define MANASCRIPT_JIT_HPP

#include <memory>
#include <string>

#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/Error.h>

namespace mana {

/**
 * @brief JIT compiler for Manascript
 * 
 * Uses LLVM's ORC JIT APIs to compile and execute LLVM IR.
 */
class JIT {
private:
    llvm::orc::ExecutionSession session;
    llvm::orc::RTDyldObjectLinkingLayer object_layer;
    llvm::orc::IRCompileLayer compile_layer;
    
    llvm::DataLayout data_layout;
    llvm::orc::MangleAndInterner mangle;
    llvm::orc::JITDylib &main_jd;
    
public:
    JIT(llvm::orc::JITTargetMachineBuilder jtmb, llvm::DataLayout dl);
    
    ~JIT();
    
    /**
     * @brief Create a new JIT instance
     * @return JIT instance or error
     */
    static llvm::Expected<std::unique_ptr<JIT>> create();
    
    /**
     * @brief Get the data layout used by the JIT
     * @return Data layout
     */
    const llvm::DataLayout& getDataLayout() const { return data_layout; }
    
    /**
     * @brief Get the main JIT dylib
     * @return JIT dylib
     */
    llvm::orc::JITDylib& getMainJITDylib() { return main_jd; }
    
    /**
     * @brief Add a module to the JIT
     * @param module LLVM module to add
     * @return Error if any
     */
    llvm::Error addModule(std::unique_ptr<llvm::Module> module);
    
    /**
     * @brief Look up a symbol in the JIT
     * @param name Symbol name
     * @return Symbol address or error
     */
    llvm::Expected<llvm::JITEvaluatedSymbol> lookup(llvm::StringRef name);
};

} // namespace mana

#endif // MANASCRIPT_JIT_HPP
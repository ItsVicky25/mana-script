#include "jit.hpp"

namespace mana {

JIT::JIT(llvm::orc::JITTargetMachineBuilder jtmb, llvm::DataLayout dl)
    : object_layer(session, []() { return std::make_unique<llvm::SectionMemoryManager>(); }),
      compile_layer(session, object_layer,
                    std::make_unique<llvm::orc::ConcurrentIRCompiler>(std::move(jtmb))),
      data_layout(std::move(dl)),
      mangle(session, data_layout),
      main_jd(session.createBareJITDylib("<main>")) {
    
    main_jd.addGenerator(
        llvm::cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
            data_layout.getGlobalPrefix())));
}

JIT::~JIT() {
    if (auto err = session.endSession())
        llvm::errs() << "JIT session end failed: " << err << "\n";
}

llvm::Expected<std::unique_ptr<JIT>> JIT::create() {
    auto jtmb = llvm::orc::JITTargetMachineBuilder::detectHost();
    
    if (!jtmb)
        return jtmb.takeError();
    
    auto dl = jtmb->getDefaultDataLayoutForTarget();
    if (!dl)
        return dl.takeError();
    
    return std::make_unique<JIT>(std::move(*jtmb), std::move(*dl));
}

llvm::Error JIT::addModule(std::unique_ptr<llvm::Module> module) {
    return compile_layer.add(main_jd, llvm::orc::ThreadSafeModule(
        std::move(module), std::make_unique<llvm::LLVMContext>()));
}

llvm::Expected<llvm::JITEvaluatedSymbol> JIT::lookup(llvm::StringRef name) {
    return session.lookup({&main_jd}, mangle(name.str()));
}

} // namespace mana
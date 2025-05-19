#include "symbol_table.hpp"

namespace mana {

bool Scope::define(const std::string& name, Symbol::Kind kind, std::shared_ptr<Type> type) {
    // Check if the symbol already exists in this scope
    if (symbols.find(name) != symbols.end()) {
        return false;
    }
    
    // Add the symbol to this scope
    symbols.emplace(name, Symbol(name, kind, type));
    return true;
}

Symbol* Scope::resolve(const std::string& name) {
    // Look in the current scope
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    
    // Look in the parent scope if it exists
    if (parent) {
        return parent->resolve(name);
    }
    
    return nullptr;
}

Symbol* Scope::resolveLocal(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    
    return nullptr;
}

void SymbolTable::enterScope() {
    current_scope = std::make_shared<Scope>(current_scope);
}

void SymbolTable::exitScope() {
    if (current_scope->getParent()) {
        current_scope = current_scope->getParent();
    }
}

bool SymbolTable::define(const std::string& name, Symbol::Kind kind, std::shared_ptr<Type> type) {
    return current_scope->define(name, kind, type);
}

Symbol* SymbolTable::resolve(const std::string& name) {
    return current_scope->resolve(name);
}

} // namespace mana
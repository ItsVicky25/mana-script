#ifndef MANASCRIPT_SYMBOL_TABLE_HPP
#define MANASCRIPT_SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <optional>

namespace mana {

// Forward declarations
class Type;

/**
 * @brief Represents a symbol (variable, function, etc.) in the symbol table
 */
class Symbol {
public:
    enum class Kind {
        VARIABLE,
        FUNCTION,
        PARAMETER
    };
    
    Symbol(const std::string& name, Kind kind, std::shared_ptr<Type> type = nullptr)
        : name(name), kind(kind), type(type) {}
    
    const std::string& getName() const { return name; }
    Kind getKind() const { return kind; }
    std::shared_ptr<Type> getType() const { return type; }
    
    void setType(std::shared_ptr<Type> type) { this->type = type; }
    
private:
    std::string name;
    Kind kind;
    std::shared_ptr<Type> type;
};

/**
 * @brief Represents a scope in the symbol table
 */
class Scope {
public:
    Scope(std::shared_ptr<Scope> parent = nullptr) : parent(parent) {}
    
    /**
     * @brief Define a symbol in the current scope
     * @param name Symbol name
     * @param kind Symbol kind
     * @param type Symbol type (optional)
     * @return True if the symbol was defined, false if it already exists
     */
    bool define(const std::string& name, Symbol::Kind kind, std::shared_ptr<Type> type = nullptr);
    
    /**
     * @brief Lookup a symbol in the current scope or parent scopes
     * @param name Symbol name
     * @return Pointer to the symbol if found, nullptr otherwise
     */
    Symbol* resolve(const std::string& name);
    
    /**
     * @brief Lookup a symbol in the current scope only
     * @param name Symbol name
     * @return Pointer to the symbol if found, nullptr otherwise
     */
    Symbol* resolveLocal(const std::string& name);
    
    /**
     * @brief Get the parent scope
     * @return Pointer to the parent scope
     */
    std::shared_ptr<Scope> getParent() const { return parent; }
    
private:
    std::unordered_map<std::string, Symbol> symbols;
    std::shared_ptr<Scope> parent;
};

/**
 * @brief Symbol table for tracking variables, functions, etc.
 */
class SymbolTable {
public:
    SymbolTable() : global_scope(std::make_shared<Scope>()) {
        current_scope = global_scope;
    }
    
    /**
     * @brief Enter a new scope
     */
    void enterScope();
    
    /**
     * @brief Exit the current scope
     */
    void exitScope();
    
    /**
     * @brief Define a symbol in the current scope
     * @param name Symbol name
     * @param kind Symbol kind
     * @param type Symbol type (optional)
     * @return True if the symbol was defined, false if it already exists
     */
    bool define(const std::string& name, Symbol::Kind kind, std::shared_ptr<Type> type = nullptr);
    
    /**
     * @brief Lookup a symbol in the current scope or parent scopes
     * @param name Symbol name
     * @return Pointer to the symbol if found, nullptr otherwise
     */
    Symbol* resolve(const std::string& name);
    
    /**
     * @brief Get the current scope
     * @return Reference to the current scope
     */
    std::shared_ptr<Scope> getCurrentScope() const { return current_scope; }
    
    /**
     * @brief Get the global scope
     * @return Reference to the global scope
     */
    std::shared_ptr<Scope> getGlobalScope() const { return global_scope; }
    
private:
    std::shared_ptr<Scope> global_scope;
    std::shared_ptr<Scope> current_scope;
};

} // namespace mana

#endif // MANASCRIPT_SYMBOL_TABLE_HPP
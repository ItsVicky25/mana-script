#ifndef MANASCRIPT_ERROR_HPP
#define MANASCRIPT_ERROR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <exception>

namespace mana {

/**
 * @brief Severity levels for diagnostic messages
 */
enum class DiagnosticSeverity {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

/**
 * @brief Represents a source location in the code
 */
struct SourceLocation {
    std::string filename;
    int line;
    int column;
    
    SourceLocation(const std::string& filename = "", int line = 0, int column = 0)
        : filename(filename), line(line), column(column) {}
    
    std::string toString() const;
};

/**
 * @brief Represents a diagnostic message for error reporting
 */
class Diagnostic {
private:
    DiagnosticSeverity severity;
    std::string message;
    SourceLocation location;
    std::string code_context;

public:
    Diagnostic(DiagnosticSeverity severity, 
               const std::string& message,
               const SourceLocation& location,
               const std::string& code_context = "")
        : severity(severity), message(message), 
          location(location), code_context(code_context) {}
    
    DiagnosticSeverity getSeverity() const { return severity; }
    const std::string& getMessage() const { return message; }
    const SourceLocation& getLocation() const { return location; }
    const std::string& getCodeContext() const { return code_context; }
    
    std::string toString() const;
};

/**
 * @brief Exception class for compiler errors
 */
class CompilerError : public std::exception {
private:
    Diagnostic diagnostic;
    
public:
    CompilerError(const Diagnostic& diagnostic) : diagnostic(diagnostic) {}
    
    const Diagnostic& getDiagnostic() const { return diagnostic; }
    
    const char* what() const noexcept override {
        return diagnostic.getMessage().c_str();
    }
};

/**
 * @brief Manages diagnostic messages for the compiler
 */
class DiagnosticManager {
private:
    std::vector<Diagnostic> diagnostics;
    bool has_errors = false;
    
public:
    void report(const Diagnostic& diagnostic);
    void report(DiagnosticSeverity severity, 
                const std::string& message,
                const SourceLocation& location,
                const std::string& code_context = "");
    
    bool hasErrors() const { return has_errors; }
    const std::vector<Diagnostic>& getDiagnostics() const { return diagnostics; }
    
    void printDiagnostics(std::ostream& os = std::cerr) const;
    void clear() { diagnostics.clear(); has_errors = false; }
};

// Global diagnostic manager instance
extern DiagnosticManager diagnostics;

} // namespace mana

#endif // MANASCRIPT_ERROR_HPP
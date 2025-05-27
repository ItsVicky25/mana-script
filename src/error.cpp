#include "error.hpp"
#include <sstream>

namespace mana {

// Initialize the global diagnostic manager
DiagnosticManager diagnostics;

std::string SourceLocation::toString() const {
    std::stringstream ss;
    if (!filename.empty()) {
        ss << filename << ":";
    }
    ss << line << ":" << column;
    return ss.str();
}

std::string Diagnostic::toString() const {
    std::string severity_str;
    switch (severity) {
        case DiagnosticSeverity::INFO:    severity_str = "info"; break;
        case DiagnosticSeverity::WARNING: severity_str = "warning"; break;
        case DiagnosticSeverity::ERROR:   severity_str = "error"; break;
        case DiagnosticSeverity::FATAL:   severity_str = "fatal error"; break;
        default: severity_str = "unknown"; break;
    }
    
    std::stringstream ss;
    ss << location.toString() << ": " << severity_str << ": " << message;
    
    if (!code_context.empty()) {
        ss << "\n" << code_context;
        
        // Add a caret pointing to the column position
        if (location.column > 0) {
            ss << "\n" << std::string(location.column - 1, ' ') << "^";
        }
    }
    
    return ss.str();
}

void DiagnosticManager::report(const Diagnostic& diagnostic) {
    diagnostics.push_back(diagnostic);
    
    if (diagnostic.getSeverity() == DiagnosticSeverity::ERROR || 
        diagnostic.getSeverity() == DiagnosticSeverity::FATAL) {
        has_errors = true;
    }
}

void DiagnosticManager::report(DiagnosticSeverity severity, 
                               const std::string& message,
                               const SourceLocation& location,
                               const std::string& code_context) {
    report(Diagnostic(severity, message, location, code_context));
}

void DiagnosticManager::printDiagnostics(std::ostream& os) const {
    for (const auto& diagnostic : diagnostics) {
        os << diagnostic.toString() << std::endl;
    }
}

} // namespace mana
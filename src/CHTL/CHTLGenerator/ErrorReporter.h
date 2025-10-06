#ifndef CHTL_ERROR_REPORTER_H
#define CHTL_ERROR_REPORTER_H

#include "../../Common.h"
#include <vector>
#include <sstream>

namespace CHTL {

enum class ErrorLevel {
    Warning,
    Error,
    Fatal
};

struct CompileError {
    ErrorLevel level;
    String message;
    String filename;
    int line;
    int column;
    String hint;
    
    CompileError(ErrorLevel lvl, const String& msg, const String& file = "", 
                 int ln = 0, int col = 0, const String& h = "")
        : level(lvl), message(msg), filename(file), line(ln), column(col), hint(h) {}
    
    String format() const {
        std::stringstream ss;
        
        // Error level
        switch (level) {
            case ErrorLevel::Warning:
                ss << "[WARNING] ";
                break;
            case ErrorLevel::Error:
                ss << "[ERROR] ";
                break;
            case ErrorLevel::Fatal:
                ss << "[FATAL] ";
                break;
        }
        
        // Location
        if (!filename.empty()) {
            ss << filename;
            if (line > 0) {
                ss << ":" << line;
                if (column > 0) {
                    ss << ":" << column;
                }
            }
            ss << " - ";
        }
        
        // Message
        ss << message;
        
        // Hint
        if (!hint.empty()) {
            ss << "\n  Hint: " << hint;
        }
        
        return ss.str();
    }
};

class ErrorReporter {
public:
    static ErrorReporter& getInstance() {
        static ErrorReporter instance;
        return instance;
    }
    
    void report(const CompileError& error) {
        errors_.push_back(error);
    }
    
    void warning(const String& message, const String& filename = "", 
                 int line = 0, int column = 0, const String& hint = "") {
        report(CompileError(ErrorLevel::Warning, message, filename, line, column, hint));
    }
    
    void error(const String& message, const String& filename = "", 
               int line = 0, int column = 0, const String& hint = "") {
        report(CompileError(ErrorLevel::Error, message, filename, line, column, hint));
    }
    
    void fatal(const String& message, const String& filename = "", 
               int line = 0, int column = 0, const String& hint = "") {
        report(CompileError(ErrorLevel::Fatal, message, filename, line, column, hint));
    }
    
    bool hasErrors() const {
        for (const auto& err : errors_) {
            if (err.level == ErrorLevel::Error || err.level == ErrorLevel::Fatal) {
                return true;
            }
        }
        return false;
    }
    
    bool hasWarnings() const {
        for (const auto& err : errors_) {
            if (err.level == ErrorLevel::Warning) {
                return true;
            }
        }
        return false;
    }
    
    const std::vector<CompileError>& getErrors() const {
        return errors_;
    }
    
    void clear() {
        errors_.clear();
    }
    
    String formatAll() const {
        std::stringstream ss;
        for (const auto& err : errors_) {
            ss << err.format() << "\n";
        }
        return ss.str();
    }
    
private:
    ErrorReporter() = default;
    std::vector<CompileError> errors_;
};

} // namespace CHTL

#endif // CHTL_ERROR_REPORTER_H

#ifndef CHTL_EXCEPTION_H
#define CHTL_EXCEPTION_H

#include <stdexcept>
#include <string>
#include <sstream>

class CHTLException : public std::runtime_error {
public:
    CHTLException(const std::string& message, size_t line, size_t column)
        : std::runtime_error(formatMessage(message, line, column)),
          line_(line),
          column_(column) {}

    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }

private:
    size_t line_;
    size_t column_;

    static std::string formatMessage(const std::string& message, size_t line, size_t column) {
        std::stringstream ss;
        ss << "Error on line " << line << ", column " << column << ": " << message;
        return ss.str();
    }
};

#endif // CHTL_EXCEPTION_H
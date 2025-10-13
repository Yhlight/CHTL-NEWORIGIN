#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace CHTL {

// C++20 概念定义
template<typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

template<typename T>
concept Printable = requires(T t, std::ostream& os) {
    { os << t } -> std::convertible_to<std::ostream&>;
};

// 前向声明
class Token;
class Node;
class Context;

// 类型别名
using String = std::string;
using StringView = std::string_view;
using Path = std::filesystem::path;

template<typename T>
using Vec = std::vector<T>;

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T>
using Opt = std::optional<T>;

template<typename K, typename V>
using Map = std::unordered_map<K, V>;

// 错误处理
class CompileError : public std::runtime_error {
public:
    CompileError(const String& message, int line = -1, int column = -1)
        : std::runtime_error(message), line_(line), column_(column) {}
    
    int line() const { return line_; }
    int column() const { return column_; }
    
private:
    int line_;
    int column_;
};

// 日志工具
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static void log(LogLevel level, const String& message) {
        switch (level) {
            case LogLevel::Debug:
                std::cout << "[DEBUG] " << message << std::endl;
                break;
            case LogLevel::Info:
                std::cout << "[INFO] " << message << std::endl;
                break;
            case LogLevel::Warning:
                std::cerr << "[WARNING] " << message << std::endl;
                break;
            case LogLevel::Error:
                std::cerr << "[ERROR] " << message << std::endl;
                break;
        }
    }
    
    static void debug(const String& message) { log(LogLevel::Debug, message); }
    static void info(const String& message) { log(LogLevel::Info, message); }
    static void warning(const String& message) { log(LogLevel::Warning, message); }
    static void error(const String& message) { log(LogLevel::Error, message); }
};

} // namespace CHTL

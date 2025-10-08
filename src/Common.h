#ifndef CHTL_COMMON_H
#define CHTL_COMMON_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>
#include <stdexcept>

namespace CHTL {

// 前向声明
class Token;
class BaseNode;
class CHTLState;
class CHTLStrategy;

// 类型别名
using String = std::string;
using StringView = std::string_view;
template<typename T>
using Vector = std::vector<T>;
template<typename T>
using SharedPtr = std::shared_ptr<T>;
template<typename T>
using UniquePtr = std::unique_ptr<T>;
template<typename T>
using Optional = std::optional<T>;
template<typename K, typename V>
using HashMap = std::unordered_map<K, V>;
template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;
template<typename... Types>
using Tuple = std::tuple<Types...>;

// 位置信息
struct Position {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;
    
    Position() = default;
    Position(size_t l, size_t c, size_t o) : line(l), column(c), offset(o) {}
    
    String toString() const {
        return "Line " + std::to_string(line) + ", Column " + std::to_string(column);
    }
};

// 源代码范围
struct SourceRange {
    Position start;
    Position end;
    
    SourceRange() = default;
    SourceRange(const Position& s, const Position& e) : start(s), end(e) {}
    
    String toString() const {
        return start.toString() + " - " + end.toString();
    }
};

// 编译错误
class CompileError : public std::runtime_error {
public:
    CompileError(const String& message, const Position& pos)
        : std::runtime_error(formatMessage(message, pos))
        , position(pos)
        , message_(message) {}
    
    const Position& getPosition() const { return position; }
    const String& getMessage() const { return message_; }
    
private:
    Position position;
    String message_;
    
    static String formatMessage(const String& msg, const Position& pos) {
        return "[" + pos.toString() + "] " + msg;
    }
};

} // namespace CHTL

#endif // CHTL_COMMON_H

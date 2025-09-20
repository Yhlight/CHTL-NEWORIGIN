#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <algorithm>
#include <regex>

namespace CHTL {

// Forward declarations
class Token;
class ASTNode;
class Context;
class Generator;

// Basic types
using String = std::string;
using StringVector = std::vector<std::string>;
using StringMap = std::map<std::string, std::string>;
using TokenVector = std::vector<std::shared_ptr<Token>>;
using ASTNodeVector = std::vector<std::shared_ptr<ASTNode>>;

// Token types
enum class TokenType {
    // Basic tokens
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    COMMENT,
    GENERATOR_COMMENT,
    
    // Operators
    COLON,
    EQUAL,
    SEMICOLON,
    COMMA,
    DOT,
    ARROW,
    
    // Brackets
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    
    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    INHERIT,
    DELETE,
    INSERT,
    FROM,
    AS,
    EXCEPT,
    USE,
    HTML5,
    
    // Special
    AT_SYMBOL,
    HASH_SYMBOL,
    DOLLAR_SYMBOL,
    AMPERSAND_SYMBOL,
    
    // Arithmetic operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    POWER,
    
    // Comparison operators
    EQUAL_TO,
    NOT_EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_EQUAL,
    GREATER_EQUAL,
    
    // Logical operators
    AND,
    OR,
    NOT,
    
    // Conditional
    QUESTION_MARK,
    
    // End of file
    EOF_TOKEN,
    
    // Unknown
    UNKNOWN
};

// Node types
enum class NodeType {
    // Basic nodes
    ELEMENT,
    TEXT,
    COMMENT,
    ATTRIBUTE,
    
    // Template nodes
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    
    // Custom nodes
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    
    // Origin nodes
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    
    // Import nodes
    IMPORT_HTML,
    IMPORT_STYLE,
    IMPORT_JAVASCRIPT,
    IMPORT_CHTL,
    IMPORT_CJMOD,
    
    // Configuration nodes
    CONFIGURATION_BLOCK,
    NAME_BLOCK,
    
    // Namespace nodes
    NAMESPACE_BLOCK,
    
    // Style nodes
    STYLE_BLOCK,
    STYLE_PROPERTY,
    
    // Script nodes
    SCRIPT_BLOCK,
    
    // Operator nodes
    DELETE_OPERATOR,
    INSERT_OPERATOR,
    INHERIT_OPERATOR
};

// Exception classes
class CHTLException : public std::exception {
private:
    String message;
    int line;
    int column;

public:
    CHTLException(const String& msg, int l = -1, int c = -1) 
        : message(msg), line(l), column(c) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    int getLine() const { return line; }
    int getColumn() const { return column; }
};

// Utility functions
class StringUtil {
public:
    static String trim(const String& str);
    static StringVector split(const String& str, char delimiter);
    static bool startsWith(const String& str, const String& prefix);
    static bool endsWith(const String& str, const String& suffix);
    static String toLowerCase(const String& str);
    static String toUpperCase(const String& str);
    static bool isIdentifier(const String& str);
    static bool isNumber(const String& str);
    static String escapeString(const String& str);
    static String unescapeString(const String& str);
};

// File system utilities
class FileSystem {
public:
    static String readFile(const String& path);
    static void writeFile(const String& path, const String& content);
    static bool fileExists(const String& path);
    static bool isDirectory(const String& path);
    static StringVector listFiles(const String& path);
    static String getExtension(const String& path);
    static String getBasename(const String& path);
    static String getDirname(const String& path);
    static String normalizePath(const String& path);
};

} // namespace CHTL
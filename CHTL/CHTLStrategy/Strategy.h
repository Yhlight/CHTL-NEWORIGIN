#pragma once
#include <string>
#include <memory>

namespace chtl {

// Strategy base interfaces per CHTL.md (no private extensions)

class CssBackendStrategy {
public:
    virtual ~CssBackendStrategy() = default;
    virtual std::string compile(const std::string& cssSource) = 0;
};

class JsBackendStrategy {
public:
    virtual ~JsBackendStrategy() = default;
    virtual std::string compile(const std::string& jsSource) = 0;
};

class PlaceholderEncodingStrategy {
public:
    virtual ~PlaceholderEncodingStrategy() = default;
    virtual std::string encodeCss(const std::string& chunk) = 0;
    virtual std::string encodeJs(const std::string& chunk) = 0;
};

class OutputStrategy {
public:
    virtual ~OutputStrategy() = default;
    virtual void writeOutput(const std::string& html,
                             const std::string& css,
                             const std::string& js) = 0;
};

} // namespace chtl


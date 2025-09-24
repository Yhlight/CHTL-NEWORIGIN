#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include <string>
#include <vector>
#include <map>
#include <optional>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// A simple struct to represent a single CSS property
struct StyleProperty {
    std::string name;
    std::string value;
};

// Represents a [Template] @Style ... definition
class TemplateStyleGroup {
public:
    explicit TemplateStyleGroup(const std::string& name) : name(name) {}

    std::string GetName() const { return name; }
    const std::vector<StyleProperty>& GetProperties() const { return properties; }

    void AddProperty(const StyleProperty& prop) { properties.push_back(prop); }

private:
    std::string name;
    std::vector<StyleProperty> properties;
    // In the future, this can be extended to support inheritance:
    // std::vector<std::string> inheritedTemplates;
};

// Represents a [Template] @Var ... definition
class TemplateVarGroup {
public:
    explicit TemplateVarGroup(const std::string& name) : name(name) {}
    TemplateVarGroup() = default; // Needed for map emplacement

    std::string GetName() const { return name; }
    const std::map<std::string, std::string>& GetVars() const { return variables; }
    std::optional<std::string> GetVar(const std::string& varName) const {
        auto it = variables.find(varName);
        if (it != variables.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void AddVar(const std::string& key, const std::string& value) { variables[key] = value; }

private:
    std::string name;
    std::map<std::string, std::string> variables;
};

// Represents a [Template] @Element ... definition
class TemplateElementGroup {
public:
    explicit TemplateElementGroup(const std::string& name) : name(name) {}
    TemplateElementGroup() = default;

    std::string GetName() const { return name; }
    const std::vector<Token>& GetTokens() const { return tokens; }

    void SetTokens(std::vector<Token> t) { tokens = std::move(t); }

private:
    std::string name;
    std::vector<Token> tokens;
};

// Represents a [Custom] @Style ... definition
class CustomStyleGroup {
public:
    explicit CustomStyleGroup(const std::string& name) : name(name) {}
    CustomStyleGroup() = default;

    std::string GetName() const { return name; }
    const std::vector<StyleProperty>& GetProperties() const { return properties; }

    void AddProperty(const StyleProperty& prop) { properties.push_back(prop); }

private:
    std::string name;
    std::vector<StyleProperty> properties;
};


} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H

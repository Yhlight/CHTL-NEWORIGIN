#pragma once

#include <string>
#include <unordered_map>
#include <memory>

// Forward declarations for future classes
// namespace CHTL { class BaseNode; }

namespace CHTL {

/**
 * @class Context
 * @brief Holds the overall state of a compilation unit.
 *
 * This includes all defined templates, custom components, variables,
 * configurations, and imported namespaces that the parser and generator
 * need to be aware of.
 */
class Context {
public:
    Context();
    ~Context();

    // In the future, this class will have methods like:
    // void addElementTemplate(const std::string& name, std::shared_ptr<BaseNode> node);
    // std::shared_ptr<BaseNode> findElementTemplate(const std::string& name) const;
    //
    // void setConfiguration(const std::string& key, const std::string& value);
    // std::string getConfiguration(const std::string& key) const;

private:
    // We will add data structures here to store the compilation state.
    // For example:
    // std::unordered_map<std::string, std::shared_ptr<BaseNode>> elementTemplates;
};

} // namespace CHTL

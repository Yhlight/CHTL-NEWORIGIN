#ifndef CHTL_TEMPLATE_VAR_NODE_H
#define CHTL_TEMPLATE_VAR_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <sstream>

class TemplateVarNode : public BaseNode {
public:
    TemplateVarNode(const std::string& name) : templateName(name) {}

    void addVariable(const std::string& key, const std::string& value) {
        variables[key] = value;
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "[Template] @Var " << templateName << " {" << std::endl;
        for (const auto& var : variables) {
            ss << std::string((depth + 1) * 2, ' ') << var.first << ": \"" << var.second << "\";" << std::endl;
        }
        ss << std::string(depth * 2, ' ') << "}" << std::endl;
        return ss.str();
    }

    const std::string& getName() const { return templateName; }
    const std::map<std::string, std::string>& getVariables() const { return variables; }

private:
    std::string templateName;
    std::map<std::string, std::string> variables;
};

#endif //CHTL_TEMPLATE_VAR_NODE_H
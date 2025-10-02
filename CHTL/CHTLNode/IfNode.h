#ifndef CHTL_IF_NODE_H
#define CHTL_IF_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class IfNode : public BaseNode {
public:
    explicit IfNode(const std::vector<Token>& conditionTokens) : conditionTokens(conditionTokens) {}

    std::string getNodeType() const override { return "If"; }

    void addProperty(const std::string& key, const std::string& value) {
        properties.push_back({key, value});
    }

    void setElseBranch(std::unique_ptr<BaseNode> else_branch) {
        this->elseBranch = std::move(else_branch);
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "if (condition) {" << std::endl; // Simplified for now
        for (const auto& prop : properties) {
            ss << std::string((depth + 1) * 2, ' ') << prop.first << ": " << prop.second << ";" << std::endl;
        }
        ss << std::string(depth * 2, ' ') << "}" << std::endl;
        if (elseBranch) {
            ss << elseBranch->toString(depth);
        }
        return ss.str();
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    const std::vector<Token>& getConditionTokens() const {
        return conditionTokens;
    }

    const std::vector<std::pair<std::string, std::string>>& getProperties() const {
        return properties;
    }

    const std::unique_ptr<BaseNode>& getElseBranch() const {
        return elseBranch;
    }

private:
    std::vector<Token> conditionTokens;
    std::vector<std::pair<std::string, std::string>> properties;
    std::unique_ptr<BaseNode> elseBranch;
};

#endif //CHTL_IF_NODE_H
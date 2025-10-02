#ifndef CHTL_CONDITION_NODE_H
#define CHTL_CONDITION_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>

class ConditionNode : public BaseNode {
public:
    explicit ConditionNode(const std::string& condition) : condition(condition) {}

    const std::string& getCondition() const { return condition; }

    std::string getNodeType() const override { return "Condition"; }

    void addProperty(const std::string& key, const std::string& value) {
        properties[key] = value;
    }

    const std::map<std::string, std::string>& getProperties() const {
        return properties;
    }

    void setNext(std::unique_ptr<ConditionNode> nextNode) {
        next = std::move(nextNode);
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "if (" << condition << ") {" << std::endl;

        for (const auto& prop : properties) {
            ss << std::string((depth + 1) * 2, ' ') << prop.first << ": " << prop.second << ";" << std::endl;
        }

        ss << std::string(depth * 2, ' ') << "}" << std::endl;

        if (next) {
            ss << next->toString(depth);
        }
        return ss.str();
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

public:
    const ConditionNode* getNext() const {
        return next.get();
    }

    ConditionNode* getNext() {
        return next.get();
    }

private:
    std::string condition;
    std::map<std::string, std::string> properties;
    std::unique_ptr<ConditionNode> next;
};

#endif //CHTL_CONDITION_NODE_H
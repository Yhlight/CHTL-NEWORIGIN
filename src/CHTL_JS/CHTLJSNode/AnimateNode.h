#pragma once

#include "CHTLJSBaseNode.h"
#include <string>
#include <map>

class AnimateNode : public CHTLJSBaseNode {
private:
    std::map<std::string, std::string> properties;

public:
    AnimateNode(const std::map<std::string, std::string>& props)
        : properties(props) {}

    const std::map<std::string, std::string>& getProperties() const {
        return properties;
    }

    virtual std::string toString() const override {
        std::string result = "AnimateNode(Properties: {";
        for (const auto& pair : properties) {
            result += " " + pair.first + ": " + pair.second + ";";
        }
        result += " })";
        return result;
    }
};
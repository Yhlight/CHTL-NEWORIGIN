#pragma once

#include "CHTLJSBaseNode.h"
#include <string>
#include <map>

class DelegateNode : public CHTLJSBaseNode {
private:
    std::string parent_selector;
    std::string target_selector;
    std::map<std::string, std::string> events;

public:
    DelegateNode(const std::string& parent, const std::string& target, const std::map<std::string, std::string>& evts)
        : parent_selector(parent), target_selector(target), events(evts) {}

    const std::string& getParentSelector() const {
        return parent_selector;
    }

    const std::string& getTargetSelector() const {
        return target_selector;
    }

    const std::map<std::string, std::string>& getEvents() const {
        return events;
    }

    virtual std::string toString() const override {
        std::string result = "DelegateNode(Parent: " + parent_selector + ", Target: " + target_selector + ", Events: {";
        for (const auto& pair : events) {
            result += " " + pair.first + ": " + pair.second + ";";
        }
        result += " })";
        return result;
    }
};
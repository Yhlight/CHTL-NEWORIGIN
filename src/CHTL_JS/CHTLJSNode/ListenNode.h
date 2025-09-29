#pragma once

#include "CHTLJSBaseNode.h"
#include <string>
#include <vector>
#include <map>

class ListenNode : public CHTLJSBaseNode {
private:
    std::string selector;
    std::map<std::string, std::string> events;

public:
    ListenNode(const std::string& sel, const std::map<std::string, std::string>& evts)
        : selector(sel), events(evts) {}

    const std::string& getSelector() const {
        return selector;
    }

    const std::map<std::string, std::string>& getEvents() const {
        return events;
    }

    virtual std::string toString() const override {
        std::string result = "ListenNode(Selector: " + selector + ", Events: {";
        for (const auto& pair : events) {
            result += " " + pair.first + ": " + pair.second + ";";
        }
        result += " })";
        return result;
    }
};
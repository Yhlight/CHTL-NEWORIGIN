#pragma once

#include "CHTLJSBaseNode.h"
#include <string>
#include <map>

class RouterNode : public CHTLJSBaseNode {
private:
    std::map<std::string, std::string> routes;

public:
    RouterNode(const std::map<std::string, std::string>& route_map)
        : routes(route_map) {}

    const std::map<std::string, std::string>& getRoutes() const {
        return routes;
    }

    virtual std::string toString() const override {
        std::string result = "RouterNode(Routes: {";
        for (const auto& pair : routes) {
            result += " " + pair.first + ": " + pair.second + ";";
        }
        result += " })";
        return result;
    }
};
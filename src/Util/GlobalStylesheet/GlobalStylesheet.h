#pragma once

#include <string>
#include <vector>

class GlobalStylesheet {
public:
    void addRule(const std::string& selector, const std::string& body);
    std::string getStylesheet() const;

private:
    std::vector<std::string> rules;
};
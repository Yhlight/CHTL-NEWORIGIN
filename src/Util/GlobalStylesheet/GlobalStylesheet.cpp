#include "GlobalStylesheet.h"
#include <sstream>

void GlobalStylesheet::addRule(const std::string& selector, const std::string& body) {
    std::stringstream rule;
    rule << selector << " { " << body << " }";
    rules.push_back(rule.str());
}

std::string GlobalStylesheet::getStylesheet() const {
    std::stringstream ss;
    for (const auto& rule : rules) {
        ss << rule << "\n";
    }
    return ss.str();
}
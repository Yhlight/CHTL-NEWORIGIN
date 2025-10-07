#pragma once

#include <string>
#include <map>
#include <stdexcept>

class TemplateRegistry {
public:
    static void registerStyleTemplate(const std::string& name, const std::string& content) {
        styleTemplates[name] = content;
    }

    static std::string getStyleTemplate(const std::string& name) {
        if (styleTemplates.find(name) == styleTemplates.end()) {
            throw std::runtime_error("Style template not found: " + name);
        }
        return styleTemplates[name];
    }

private:
    static std::map<std::string, std::string> styleTemplates;
};
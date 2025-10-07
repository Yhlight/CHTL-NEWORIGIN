#pragma once

#include <string>
#include <map>
#include <stdexcept>

using StyleTemplate = std::map<std::string, std::string>;

class TemplateRegistry {
public:
    static void registerStyleTemplate(const std::string& name, StyleTemplate content) {
        styleTemplates[name] = content;
    }

    static StyleTemplate getStyleTemplate(const std::string& name) {
        if (styleTemplates.find(name) == styleTemplates.end()) {
            throw std::runtime_error("Style template not found: " + name);
        }
        return styleTemplates.at(name);
    }

private:
    static std::map<std::string, StyleTemplate> styleTemplates;
};
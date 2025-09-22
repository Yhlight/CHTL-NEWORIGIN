#pragma once

#include <map>
#include <string>
#include <memory>

// A simple struct to hold the properties of a defined style template
struct StyleTemplate {
    std::string name;
    std::map<std::string, std::string> properties;
};

/*
 * The ParseContext holds the state during the parsing phase, primarily
 * acting as a symbol table for storing defined templates.
 */
class ParseContext {
public:
    void addStyleTemplate(const std::string& name, StyleTemplate&& styleTemplate) {
        styleTemplates[name] = std::make_shared<StyleTemplate>(std::move(styleTemplate));
    }

    std::shared_ptr<StyleTemplate> findStyleTemplate(const std::string& name) {
        if (styleTemplates.count(name)) {
            return styleTemplates.at(name);
        }
        return nullptr;
    }

private:
    std::map<std::string, std::shared_ptr<StyleTemplate>> styleTemplates;
    // Maps for element and var templates will be added later
};

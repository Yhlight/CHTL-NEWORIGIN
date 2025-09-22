#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>
#include "../CHTLNode/BaseNode.h"

// A simple struct to hold the properties of a defined style template
struct StyleTemplate {
    std::string name;
    std::map<std::string, std::string> properties;
};

// Represents a defined element template
struct ElementTemplate {
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> nodes;
};

// Represents a defined variable group template
struct VarTemplate {
    std::string name;
    std::map<std::string, std::string> variables;
};

/*
 * The ParseContext holds the state during the parsing phase, primarily
 * acting as a symbol table for storing defined templates.
 */
class ParseContext {
public:
    // Style Templates
    void addStyleTemplate(const std::string& name, StyleTemplate&& styleTemplate) {
        styleTemplates[name] = std::make_shared<StyleTemplate>(std::move(styleTemplate));
    }

    std::shared_ptr<StyleTemplate> findStyleTemplate(const std::string& name) {
        if (styleTemplates.count(name)) {
            return styleTemplates.at(name);
        }
        return nullptr;
    }

    // Element Templates
    void addElementTemplate(const std::string& name, ElementTemplate&& elementTemplate) {
        elementTemplates[name] = std::make_shared<ElementTemplate>(std::move(elementTemplate));
    }

    std::shared_ptr<ElementTemplate> findElementTemplate(const std::string& name) {
        if (elementTemplates.count(name)) {
            return elementTemplates.at(name);
        }
        return nullptr;
    }

    // Variable Templates
    void addVarTemplate(const std::string& name, VarTemplate&& varTemplate) {
        varTemplates[name] = std::make_shared<VarTemplate>(std::move(varTemplate));
    }

    std::shared_ptr<VarTemplate> findVarTemplate(const std::string& name) {
        if (varTemplates.count(name)) {
            return varTemplates.at(name);
        }
        return nullptr;
    }

private:
    std::map<std::string, std::shared_ptr<StyleTemplate>> styleTemplates;
    std::map<std::string, std::shared_ptr<ElementTemplate>> elementTemplates;
    std::map<std::string, std::shared_ptr<VarTemplate>> varTemplates;
};

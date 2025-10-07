#pragma once

#include "../CHTLNode/Node.h"
#include <string>
#include <map>
#include <stdexcept>

using StyleTemplate = std::map<std::string, std::string>;

class TemplateRegistry {
public:
    // Style Templates
    static void registerStyleTemplate(const std::string& name, StyleTemplate content) {
        styleTemplates[name] = content;
    }

    static StyleTemplate getStyleTemplate(const std::string& name) {
        if (styleTemplates.find(name) == styleTemplates.end()) {
            throw std::runtime_error("Style template not found: " + name);
        }
        return styleTemplates.at(name);
    }

    // Element Templates
    static void registerElementTemplate(const std::string& name, const NodeList& content) {
        elementTemplates[name] = content;
    }

    static NodeList getElementTemplate(const std::string& name) {
        if (elementTemplates.find(name) == elementTemplates.end()) {
            throw std::runtime_error("Element template not found: " + name);
        }
        return elementTemplates.at(name);
    }

private:
    static std::map<std::string, StyleTemplate> styleTemplates;
    static std::map<std::string, NodeList> elementTemplates;
};
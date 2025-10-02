#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "../CHTLNode/TemplateStyleNode.h"
#include "../CHTLNode/TemplateElementNode.h"
#include "../CHTLNode/TemplateVarNode.h"
#include "../CHTLNode/CustomStyleNode.h"
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/CustomVarNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>

// Forward declaration
class CHTLContext;

// Represents a single namespace or scope
struct Scope {
    std::map<std::string, std::unique_ptr<TemplateStyleNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<TemplateElementNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<TemplateVarNode>> varTemplates;
    std::map<std::string, std::unique_ptr<CustomStyleNode>> customStyleTemplates;
    std::map<std::string, std::unique_ptr<CustomElementNode>> customElementTemplates;
    std::map<std::string, std::unique_ptr<CustomVarNode>> customVarTemplates;

    std::map<std::string, std::unique_ptr<Scope>> nestedScopes;
    Scope* parent = nullptr;

    Scope(Scope* p = nullptr) : parent(p) {}
};

class CHTLContext {
public:
    CHTLContext() {
        globalScope = std::make_unique<Scope>();
        scopeStack.push_back(globalScope.get());
    }

    void enterNamespace(const std::string& name) {
        Scope* currentScope = scopeStack.back();
        auto it = currentScope->nestedScopes.find(name);
        if (it == currentScope->nestedScopes.end()) {
            auto newScope = std::make_unique<Scope>(currentScope);
            it = currentScope->nestedScopes.emplace(name, std::move(newScope)).first;
        }
        scopeStack.push_back(it->second.get());
    }

    void leaveNamespace() {
        if (scopeStack.size() > 1) {
            scopeStack.pop_back();
        } else {
            throw std::runtime_error("Cannot leave the global namespace.");
        }
    }

    // Registration methods add to the current scope
    void registerStyleTemplate(std::unique_ptr<TemplateStyleNode> node) {
        scopeStack.back()->styleTemplates[node->getName()] = std::move(node);
    }
    void registerElementTemplate(std::unique_ptr<TemplateElementNode> node) {
        scopeStack.back()->elementTemplates[node->getName()] = std::move(node);
    }
    void registerVarTemplate(std::unique_ptr<TemplateVarNode> node) {
        scopeStack.back()->varTemplates[node->getName()] = std::move(node);
    }
    void registerCustomStyle(std::unique_ptr<CustomStyleNode> node) {
        scopeStack.back()->customStyleTemplates[node->getName()] = std::move(node);
    }
    void registerCustomElement(std::unique_ptr<CustomElementNode> node) {
        scopeStack.back()->customElementTemplates[node->getName()] = std::move(node);
    }
    void registerCustomVar(std::unique_ptr<CustomVarNode> node) {
        scopeStack.back()->customVarTemplates[node->getName()] = std::move(node);
    }

    // Unqualified getter methods search from the current scope up to the global scope
    const TemplateStyleNode* getStyleTemplate(const std::string& name) const {
        return getFromScope(name, scopeStack.back(), &Scope::styleTemplates);
    }
    const TemplateElementNode* getElementTemplate(const std::string& name) const {
        return getFromScope(name, scopeStack.back(), &Scope::elementTemplates);
    }
    const TemplateVarNode* getVarTemplate(const std::string& name) const {
        return getFromScope(name, scopeStack.back(), &Scope::varTemplates);
    }
    const CustomStyleNode* getCustomStyle(const std::string& name) const {
        return getFromScope(name, scopeStack.back(), &Scope::customStyleTemplates);
    }
    const CustomElementNode* getCustomElement(const std::string& name) const {
        return getFromScope(name, scopeStack.back(), &Scope::customElementTemplates);
    }
    const CustomVarNode* getCustomVar(const std::string& name) const {
        return getFromScope(name, scopeStack.back(), &Scope::customVarTemplates);
    }

    // Qualified getter methods
    const TemplateStyleNode* getStyleTemplate(const std::string& name, const std::string& path) const {
        const Scope* targetScope = findScopeByPath(path);
        return getFromScope(name, targetScope, &Scope::styleTemplates);
    }

private:
    template<typename T>
    const T* getFromScope(const std::string& name, const Scope* startScope, std::map<std::string, std::unique_ptr<T>> Scope::*member) const {
        const Scope* currentScope = startScope;
        while (currentScope) {
            const auto& container = currentScope->*member;
            auto it = container.find(name);
            if (it != container.end()) {
                return it->second.get();
            }
            currentScope = currentScope->parent;
        }
        return nullptr;
    }

    Scope* findScopeByPath(const std::string& path) const {
        Scope* currentScope = globalScope.get();
        std::stringstream ss(path);
        std::string segment;
        while(std::getline(ss, segment, '.')) {
            auto it = currentScope->nestedScopes.find(segment);
            if (it == currentScope->nestedScopes.end()) {
                return nullptr;
            }
            currentScope = it->second.get();
        }
        return currentScope;
    }

    std::unique_ptr<Scope> globalScope;
    std::vector<Scope*> scopeStack;
};

#endif //CHTL_CONTEXT_H
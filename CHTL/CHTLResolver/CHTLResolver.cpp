#include "CHTLResolver.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ElementSpecializationNode.h"
#include "../CHTLNode/DeleteElementNode.h"
#include <iostream>
#include <algorithm>

// A simple clone function for nodes that don't have children to resolve
template <typename T>
std::shared_ptr<T> cloneNode(const std::shared_ptr<BaseNode>& node) {
    auto original = std::dynamic_pointer_cast<T>(node);
    return std::make_shared<T>(*original);
}

#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/InsertElementNode.h"

CHTLResolver::CHTLResolver(CHTLContext& context) : context(context) {}

void CHTLResolver::processImports(const std::shared_ptr<ElementNode>& root, const std::string& currentFilePath) {
    std::vector<std::shared_ptr<ImportNode>> importNodes;

    // First, find all import nodes
    for (const auto& node : root->children) {
        if (node->getType() == NodeType::Import) {
            importNodes.push_back(std::dynamic_pointer_cast<ImportNode>(node));
        }
    }

    for (const auto& importNode : importNodes) {
        if (importNode->type == ImportType::Chtl) {
            try {
                std::string content = CHTLLoader::loadFile(importNode->path, currentFilePath);

                CHTLContext importedContext;
                CHTLLexer importedLexer(content);
                CHTLParser importedParser(importedLexer, importedContext, importNode->path);

                auto importedAST = importedParser.parse(); // This populates the importedContext

                // Determine the namespace for the imported file
                std::string importNamespace;
                bool foundNamespace = false;
                if (importedAST && !importedAST->children.empty()) {
                    if (auto nsNode = std::dynamic_pointer_cast<NamespaceNode>(importedAST->children[0])) {
                        importNamespace = nsNode->name;
                        foundNamespace = true;
                    }
                }
                if (!foundNamespace) {
                    importNamespace = std::filesystem::path(importNode->path).stem().string();
                }

                // Merge definitions with namespace prefix
                auto mergeWithNamespace = [&](auto& targetMap, const auto& sourceMap) {
                    for (const auto& pair : sourceMap) {
                        std::string key = importNamespace + "::" + pair.first;
                        targetMap[key] = pair.second;
                    }
                };

                mergeWithNamespace(context.styleTemplates, importedContext.styleTemplates);
                mergeWithNamespace(context.elementTemplates, importedContext.elementTemplates);
                mergeWithNamespace(context.varTemplates, importedContext.varTemplates);
                mergeWithNamespace(context.styleCustoms, importedContext.styleCustoms);
                mergeWithNamespace(context.elementCustoms, importedContext.elementCustoms);
                mergeWithNamespace(context.varCustoms, importedContext.varCustoms);

            } catch (const std::runtime_error& e) {
                std::cerr << "Warning: Failed to process import for '" << importNode->path << "'. Reason: " << e.what() << std::endl;
            }
        }
    }

    // After processing, remove import nodes from the AST so they aren't processed further
    root->children.erase(
        std::remove_if(root->children.begin(), root->children.end(),
            [](const std::shared_ptr<BaseNode>& node) {
                return node->getType() == NodeType::Import;
            }),
        root->children.end());
}


std::shared_ptr<ElementNode> CHTLResolver::resolve(const std::shared_ptr<ElementNode>& root, const std::string& currentFilePath) {
    if (!root) return nullptr;

    // 1. Process all imports first to populate the context
    processImports(root, currentFilePath);

    // 2. Resolve inheritance between templates
    resolveInheritance();

    // 3. Validate constraints on the pre-resolution tree
    validateNodeConstraints(root);

    // 4. Now resolve the main tree with the populated context
    auto resolved_nodes = resolveNode(root);
    if (resolved_nodes.empty()) return nullptr;
    return std::dynamic_pointer_cast<ElementNode>(resolved_nodes[0]);
}

std::vector<std::shared_ptr<BaseNode>> CHTLResolver::resolveNode(const std::shared_ptr<BaseNode>& node) {
    if (!node) return {};

    switch (node->getType()) {
        case NodeType::Element: {
            return { resolveElement(std::dynamic_pointer_cast<ElementNode>(node)) };
        }
        case NodeType::TemplateUsage: {
            auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node);
            if (usageNode->templateType == TemplateType::Element) {
                std::vector<std::shared_ptr<BaseNode>> childrenToProcess;
                bool found = false;

                // Get the initial list of children from the definition
                // Try to resolve in the current namespace first, then globally.
                std::string namespacedName;
                if (!context.namespaceStack.empty()) {
                    namespacedName = context.namespaceStack.back() + "::" + usageNode->name;
                }

                if (!namespacedName.empty() && context.elementTemplates.count(namespacedName)) {
                    childrenToProcess = context.elementTemplates[namespacedName]->children;
                    found = true;
                } else if (context.elementTemplates.count(usageNode->name)) {
                    childrenToProcess = context.elementTemplates[usageNode->name]->children;
                    found = true;
                } else if (!namespacedName.empty() && context.elementCustoms.count(namespacedName)) {
                    childrenToProcess = context.elementCustoms[namespacedName]->children;
                    found = true;
                }
                else if (context.elementCustoms.count(usageNode->name)) {
                    childrenToProcess = context.elementCustoms[usageNode->name]->children;
                    found = true;
                }

                if (!found) {
                     std::cerr << "Warning: Undefined element template or custom used: " << usageNode->name << std::endl;
                     return {};
                }

                // Apply specializations if they exist
                if (usageNode->specialization) {
                    for (const auto& instruction : usageNode->specialization->instructions) {
                        if (instruction->getType() == NodeType::DeleteElement) {
                            auto deleteInstruction = std::dynamic_pointer_cast<DeleteElementNode>(instruction);
                            childrenToProcess.erase(
                                std::remove_if(childrenToProcess.begin(), childrenToProcess.end(),
                                    [&](const auto& child) {
                                        if (auto elem = std::dynamic_pointer_cast<ElementNode>(child)) {
                                            return elem->tagName == deleteInstruction->targetSelector;
                                        }
                                        return false;
                                    }),
                                childrenToProcess.end());
                        } else if (instruction->getType() == NodeType::InsertElement) {
                            auto insertInstruction = std::dynamic_pointer_cast<InsertElementNode>(instruction);
                            auto it = std::find_if(childrenToProcess.begin(), childrenToProcess.end(),
                                [&](const auto& child) {
                                    if (auto elem = std::dynamic_pointer_cast<ElementNode>(child)) {
                                        return elem->tagName == insertInstruction->targetSelector;
                                    }
                                    return false;
                                });

                            if (it != childrenToProcess.end()) {
                                switch (insertInstruction->position) {
                                    case InsertPosition::After:
                                        childrenToProcess.insert(std::next(it), insertInstruction->nodesToInsert.begin(), insertInstruction->nodesToInsert.end());
                                        break;
                                    case InsertPosition::Before:
                                        childrenToProcess.insert(it, insertInstruction->nodesToInsert.begin(), insertInstruction->nodesToInsert.end());
                                        break;
                                    case InsertPosition::Replace:
                                        it = childrenToProcess.erase(it);
                                        childrenToProcess.insert(it, insertInstruction->nodesToInsert.begin(), insertInstruction->nodesToInsert.end());
                                        break;
                                    default: // AtTop and AtBottom not handled yet
                                        break;
                                }
                            }
                        }
                    }
                }

                // Finally, resolve the (potentially modified) children
                return resolveChildren(childrenToProcess);
            }
            return {}; // Other template types are not nodes
        }
        case NodeType::Text: {
            return { cloneNode<TextNode>(node) };
        }
        case NodeType::Comment: {
             return { cloneNode<CommentNode>(node) };
        }
        case NodeType::Origin: {
            return { cloneNode<OriginNode>(node) };
        }
        case NodeType::Namespace: {
            auto nsNode = std::dynamic_pointer_cast<NamespaceNode>(node);
            context.namespaceStack.push_back(nsNode->name);
            auto resolvedChildren = resolveChildren(nsNode->children);
            context.namespaceStack.pop_back();
            return resolvedChildren;
        }
        default:
            return {}; // Return empty vector for unknown or irrelevant nodes
    }
}

std::shared_ptr<ElementNode> CHTLResolver::resolveElement(const std::shared_ptr<ElementNode>& node) {
    // Create a new element, cloning properties
    auto newElement = std::make_shared<ElementNode>(node->tagName);
    newElement->attributes = node->attributes;
    newElement->inlineStyles = node->inlineStyles; // Copy existing styles first

    // --- Resolve @Style usages ---
    for (const auto& usage : node->styleUsages) {
        std::string namespacedName;
        if (!context.namespaceStack.empty()) {
            namespacedName = context.namespaceStack.back() + "::" + usage->name;
        }

        bool found = false;
        if (!namespacedName.empty() && context.styleTemplates.count(namespacedName)) {
            auto templateDef = context.styleTemplates[namespacedName];
            for (const auto& pair : templateDef->styleProperties) {
                newElement->inlineStyles[pair.first] = pair.second;
            }
            found = true;
        } else if (context.styleTemplates.count(usage->name)) {
            auto templateDef = context.styleTemplates[usage->name];
            for (const auto& pair : templateDef->styleProperties) {
                newElement->inlineStyles[pair.first] = pair.second;
            }
            found = true;
        }

        if (!namespacedName.empty() && context.styleCustoms.count(namespacedName)) {
             auto customDef = context.styleCustoms[namespacedName];
            for (const auto& pair : customDef->styleProperties) {
                newElement->inlineStyles[pair.first] = pair.second;
            }
            found = true;
        } else if (context.styleCustoms.count(usage->name)) {
            auto customDef = context.styleCustoms[usage->name];
            for (const auto& pair : customDef->styleProperties) {
                newElement->inlineStyles[pair.first] = pair.second;
            }
            found = true;
        }

        if (!found) {
             std::cerr << "Warning: Undefined style template or custom: " << usage->name << std::endl;
        }

        // After merging, apply deletions
        for (const auto& propToDelete : usage->deletedProperties) {
            newElement->inlineStyles.erase(propToDelete);
        }
    }

    // Resolve the children
    newElement->children = resolveChildren(node->children);

    return newElement;
}

std::vector<std::shared_ptr<BaseNode>> CHTLResolver::resolveChildren(const std::vector<std::shared_ptr<BaseNode>>& children) {
    std::vector<std::shared_ptr<BaseNode>> resolvedChildren;
    for (const auto& child : children) {
        auto resolvedNodes = resolveNode(child);
        // Insert all the resolved nodes (could be 1, or many if a template was expanded)
        resolvedChildren.insert(resolvedChildren.end(), resolvedNodes.begin(), resolvedNodes.end());
    }
    return resolvedChildren;
}


void CHTLResolver::validateNodeConstraints(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    // We only care about ElementNodes that can have children and constraints
    if (node->getType() != NodeType::Element) {
        return;
    }

    auto element = std::dynamic_pointer_cast<ElementNode>(node);
    if (element->constraints.empty()) {
        // No constraints on this node, but we still need to check its children
        for (const auto& child : element->children) {
            validateNodeConstraints(child);
        }
        return;
    }

    // This node has constraints, check every child against every constraint
    for (const auto& child : element->children) {
        for (const auto& constraint : element->constraints) {
            bool violation = false;
            switch (constraint.type) {
                case ConstraintType::ExactTag:
                    if (child->getType() == NodeType::Element && std::dynamic_pointer_cast<ElementNode>(child)->tagName == constraint.identifier) {
                        violation = true;
                    }
                    break;
                case ConstraintType::AnyHtml:
                    if (child->getType() == NodeType::Element) {
                        violation = true;
                    }
                    break;
                case ConstraintType::AnyTemplate:
                    if (child->getType() == NodeType::TemplateUsage) {
                        violation = true;
                    }
                    break;
                case ConstraintType::AnyCustom:
                     if (child->getType() == NodeType::TemplateUsage) {
                        auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(child);
                        if(context.elementCustoms.count(usage->name)) { // This check is basic, needs namespace handling later
                             violation = true;
                        }
                    }
                    break;
                // Other constraint types can be added here
                default:
                    break;
            }
            if (violation) {
                // In a real compiler, you'd want line/column numbers from the node
                throw std::runtime_error("Constraint violation: Element '" + element->tagName + "' cannot contain the specified child.");
            }
        }
    }

    // Recurse to check children's constraints
    for (const auto& child : element->children) {
        validateNodeConstraints(child);
    }
}

void CHTLResolver::resolveInheritance() {
    // This is a simple, one-pass implementation. A more robust version
    // would handle multi-level and circular inheritance by looping until
    // no more changes occur.
    for (auto& pair : context.styleTemplates) {
        auto& childTemplate = pair.second;
        if (childTemplate->inheritedTemplates.empty()) {
            continue;
        }

        std::map<std::string, std::shared_ptr<BaseExprNode>> newProperties;

        for (const auto& parentName : childTemplate->inheritedTemplates) {
            // For now, we assume global namespace for inheritance.
            // A full implementation would check namespaces.
            if (context.styleTemplates.count(parentName)) {
                auto& parentTemplate = context.styleTemplates[parentName];
                // Recurse to ensure parent is resolved first (for multi-level)
                // This is still tricky. A better approach is topological sort or looping.
                // For now, let's assume one level of inheritance.
                for (const auto& parentProp : parentTemplate->styleProperties) {
                    newProperties[parentProp.first] = parentProp.second;
                }
            } else {
                std::cerr << "Warning: Parent style template '" << parentName << "' not found for child '" << childTemplate->name << "'." << std::endl;
            }
        }

        // Add the child's own properties, potentially overwriting parent's
        for (const auto& childProp : childTemplate->styleProperties) {
            newProperties[childProp.first] = childProp.second;
        }

        childTemplate->styleProperties = newProperties;
        // Clear the inheritance list so we don't re-process
        childTemplate->inheritedTemplates.clear();
    }
}

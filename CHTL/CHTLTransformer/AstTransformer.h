#pragma once

#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include <memory>
#include <map>
#include <string>

// This class will be responsible for processing the raw AST from the parser
// and producing a final, expanded AST ready for code generation.
class AstTransformer {
public:
    AstTransformer() = default;
    std::unique_ptr<RootNode> transform(std::unique_ptr<RootNode> root);

private:
    // We will use raw pointers here because the transformer does not own the nodes.
    // The AST itself (via unique_ptr in the RootNode) owns them.
    std::map<std::string, TemplateDefinitionNode*> styleTemplates;
    std::map<std::string, TemplateDefinitionNode*> elementTemplates;
    std::map<std::string, std::map<std::string, std::string>> varTemplates;

    void collectTemplates(Node* node);
    void expandTemplates(Node* node);
    void expandTemplatesForChildren(Node* containerNode);
    void expandStyleTemplates(ElementNode* elementNode);
};

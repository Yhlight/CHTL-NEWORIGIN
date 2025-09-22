#pragma once

#include "Node.h"
#include "RawBodyNode.h"

enum class TemplateType {
    Style,
    Element,
    Var
};

class TemplateDefinitionNode : public Node {
public:
    TemplateDefinitionNode(TemplateType type, const std::string& name, std::unique_ptr<RawBodyNode> body)
        : templateType(type), name(name), body(std::move(body)) {}

    void accept(Visitor& visitor) override {
        // The generator will not visit this directly.
        // It's handled by the transformer.
    }

    TemplateType getTemplateType() const { return templateType; }
    std::string getName() const { return name; }
    const RawBodyNode* getBody() const { return body.get(); }

private:
    TemplateType templateType;
    std::string name;
    std::unique_ptr<RawBodyNode> body;
};

#pragma once

#include "../Node/BaseNode.h"
#include "../Node/ElementNode.h"
#include "../Node/TextNode.h"
#include <string>
#include "../Common/TemplateStore.h"

namespace CHTL {

class Generator {
public:
    Generator(TemplateStore& store);
    std::string generate(ElementNode& root);

private:
    TemplateStore& store;
    std::string global_css;

    void generateNode(BaseNode& node, std::string& output, int indent = 0);
    void generateElementNode(ElementNode& node, std::string& output, int indent);
    void generateTextNode(const TextNode& node, std::string& output, int indent);
};

} // namespace CHTL

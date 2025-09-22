#pragma once

#include "CHTL/CHTLNode/Node.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class Generator {
public:
    Generator();
    std::string generate(const std::vector<std::unique_ptr<BaseNode>>& ast);

private:
    std::stringstream output;
    int indent_level = 0;

    void generate_node(const BaseNode* node);
    void generate_element(const ElementNode* node);
    void generate_text(const TextNode* node);
    void generate_comment(const CommentNode* node);

    void write_indent();
};

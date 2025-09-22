#pragma once

#include "CHTL/CHTLNode/Node.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

struct GeneratedOutput {
    std::string html;
    std::string css;
};

class Generator {
public:
    Generator();
    GeneratedOutput generate(const std::vector<std::unique_ptr<BaseNode>>& ast);

private:
    std::stringstream html_output;
    std::stringstream css_output;
    int indent_level = 0;

    void generate_node(const BaseNode* node);
    void generate_element(const ElementNode* node);
    void generate_text(const TextNode* node);
    void generate_comment(const CommentNode* node);

    void write_indent();
};

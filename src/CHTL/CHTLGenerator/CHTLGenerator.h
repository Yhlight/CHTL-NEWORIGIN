#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "CHTL/CHTLNode/ElementNode.h"
#include <string>

namespace CHTL {

struct CompilationOutput {
    std::string html;
    std::string css;
};

class CHTLGenerator {
public:
    CHTLGenerator();
    CompilationOutput generate(const ElementNode& root);

private:
    void generateElement(const ElementNode& element, std::string& html_output, std::string& css_output, int indentation);
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H

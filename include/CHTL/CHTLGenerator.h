#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>

namespace CHTL {

/**
 * @brief CHTL code generator for HTML, CSS, and JavaScript output
 */
class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator() = default;

    // Code generation
    std::string generateHTML(BaseNode* root) const;
    std::string generateCSS(BaseNode* root) const;
    std::string generateJS(BaseNode* root) const;
    std::string generateAll(BaseNode* root) const;

    // Configuration
    void setInlineOutput(bool inlineOutput) { inlineOutput_ = inlineOutput; }
    void setMinifyOutput(bool minifyOutput) { minifyOutput_ = minifyOutput; }

private:
    bool inlineOutput_;
    bool minifyOutput_;
};

} // namespace CHTL
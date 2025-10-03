#include "CHTL/CHTLGenerator.h"

namespace CHTL {

CHTLGenerator::CHTLGenerator() : inlineOutput_(false), minifyOutput_(false) {
}

std::string CHTLGenerator::generateHTML(BaseNode* root) const {
    if (!root) {
        return "";
    }
    return root->generateHTML();
}

std::string CHTLGenerator::generateCSS(BaseNode* root) const {
    if (!root) {
        return "";
    }
    return root->generateCSS();
}

std::string CHTLGenerator::generateJS(BaseNode* root) const {
    if (!root) {
        return "";
    }
    return root->generateJS();
}

std::string CHTLGenerator::generateAll(BaseNode* root) const {
    std::string html = generateHTML(root);
    std::string css = generateCSS(root);
    std::string js = generateJS(root);
    
    std::string result;
    if (!html.empty()) {
        result += "<!-- HTML -->\n" + html + "\n\n";
    }
    if (!css.empty()) {
        result += "<!-- CSS -->\n<style>\n" + css + "\n</style>\n\n";
    }
    if (!js.empty()) {
        result += "<!-- JavaScript -->\n<script>\n" + js + "\n</script>\n";
    }
    
    return result;
}

} // namespace CHTL
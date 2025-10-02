#include "Generator.h"
#include "CHTL/CHTLContext.h"
namespace CHTL {
Generator::Generator(CHTLContext& context) : context_(context) {}

std::string Generator::generateHTML(const std::shared_ptr<BaseNode>& root) {
    return root ? root->toHTML() : "";
}

std::string Generator::generateCSS(const std::shared_ptr<BaseNode>& root) {
    return root ? root->toCSS() : "";
}

std::string Generator::generateJS(const std::shared_ptr<BaseNode>& root) {
    return root ? root->toJS() : "";
}
}

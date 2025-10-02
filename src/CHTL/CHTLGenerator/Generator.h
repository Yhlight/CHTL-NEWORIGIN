#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H
#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <memory>
namespace CHTL {
class CHTLContext;
class Generator {
public:
    Generator(CHTLContext& context);
    std::string generateHTML(const std::shared_ptr<BaseNode>& root);
    std::string generateCSS(const std::shared_ptr<BaseNode>& root);
    std::string generateJS(const std::shared_ptr<BaseNode>& root);
private:
    [[maybe_unused]] CHTLContext& context_;
};
} // namespace CHTL
#endif

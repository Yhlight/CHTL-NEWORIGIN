#ifndef CHTL_DOCUMENT_TRAVERSER_H
#define CHTL_DOCUMENT_TRAVERSER_H

#include <string>
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/BaseNode.h"

namespace CHTL {

class DocumentTraverser {
public:
    static const ElementNode* find(const BaseNode* root, const std::string& selector);
};

} // namespace CHTL

#endif // CHTL_DOCUMENT_TRAVERSER_H
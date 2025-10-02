/**
 * @file TemplateNode.h
 * @brief Template node for template definitions
 */

#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include "BaseNode.h"

namespace CHTL {

class TemplateNode : public BaseNode {
public:
    TemplateNode();
    std::string toString() const override;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H

#ifndef CHTL_STYLE_PROPERTY_NODE_H
#define CHTL_STYLE_PROPERTY_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}

    const std::string& getKey() const { return key; }
    const std::string& getValue() const { return value; }
    void print(int indent = 0) const override;

private:
    std::string key;
    std::string value;
};

} // namespace CHTL

#endif // CHTL_STYLE_PROPERTY_NODE_H
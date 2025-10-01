#ifndef CHTL_RAW_SCRIPT_NODE_H
#define CHTL_RAW_SCRIPT_NODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class RawScriptNode : public BaseNode {
public:
    explicit RawScriptNode(const std::string& script);
    void print(int level = 0) const override;
    const std::string& getScript() const;

private:
    std::string m_script;
};

} // namespace CHTL

#endif // CHTL_RAW_SCRIPT_NODE_H
#ifndef CHTL_IMPORT_NODE_H
#define CHTL_IMPORT_NODE_H

#include "../BaseNode.h"

namespace CHTL {

class ImportNode : public BaseNode {
public:
    ImportNode(const String& path, const SourceRange& range = SourceRange());

    const String& getPath() const { return path_; }
    const String& getAlias() const { return alias_; }

    void setAlias(const String& alias) { alias_ = alias; }

    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;

private:
    String path_;
    String alias_;
};

} // namespace CHTL

#endif // CHTL_IMPORT_NODE_H

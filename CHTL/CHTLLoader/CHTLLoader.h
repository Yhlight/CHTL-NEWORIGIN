#pragma once

#include "../CHTLNode/BaseNode.h"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace CHTL {

class CHTLLoader {
public:
    CHTLLoader(const std::string& basePath);
    void loadImports(const std::shared_ptr<BaseNode>& ast);
    const std::map<std::string, std::shared_ptr<BaseNode>>& getLoadedAsts() const;

private:
    void findAndLoad(const std::shared_ptr<BaseNode>& node);
    std::string resolvePath(const std::string& path);

    std::string basePath;
    std::map<std::string, std::shared_ptr<BaseNode>> loadedAsts;
};

}
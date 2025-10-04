#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLContext/GenerationContext.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <filesystem>

namespace CHTL {

class CHTLLoader {
public:
    CHTLLoader(const std::string& basePath);
    void loadImports(const std::shared_ptr<BaseNode>& ast);
    const std::map<std::string, std::shared_ptr<BaseNode>>& getLoadedAsts() const;
    const std::map<std::string, std::shared_ptr<OriginNode>>& getNamedOriginNodes() const;
    const std::map<std::string, std::shared_ptr<BaseNode>>& getImportedItems() const;
    const std::map<std::string, std::map<std::string, std::shared_ptr<BaseNode>>>& getNamespaces() const;
    void gatherTemplates(const std::shared_ptr<BaseNode>& ast, GenerationContext& context);

private:
    void findAndLoad(const std::shared_ptr<BaseNode>& node);
    std::string resolvePath(const std::string& importPath);
    std::vector<std::shared_ptr<BaseNode>> findNodes(const std::shared_ptr<BaseNode>& ast, ImportCategory category, const std::string& itemType, const std::string& itemName);


    std::string basePath;
    std::string officialModulePath;
    std::map<std::string, std::shared_ptr<BaseNode>> loadedAsts;
    std::map<std::string, std::shared_ptr<OriginNode>> namedOriginNodes;
    std::map<std::string, std::shared_ptr<BaseNode>> importedItems;
    std::map<std::string, std::map<std::string, std::shared_ptr<BaseNode>>> namespaces;
};

}
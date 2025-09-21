#pragma once

#include "Loader.h"
#include "CHTLNode/Nodes.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class Compiler {
public:
    Compiler();
    void compile(const std::string& entryFilePath);

private:
    void processFile(const std::string& filePath);

    Loader loader;
    std::vector<NodePtr> ast;
    std::vector<std::string> processedFiles;
    std::map<std::string, std::shared_ptr<TemplateNode>> templateTable;
};

} // namespace CHTL

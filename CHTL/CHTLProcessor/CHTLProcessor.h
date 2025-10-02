#ifndef CHTL_PROCESSOR_H
#define CHTL_PROCESSOR_H

#include "../CHTLContext/CHTLContext.h"
#include "../CHTLParser/CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <memory>

class CHTLProcessor {
public:
    explicit CHTLProcessor(const std::string& entryFilePath);
    std::unique_ptr<DocumentNode> process();

private:
    void processFile(const std::string& filePath);

    CHTLContext context;
    std::queue<std::string> fileQueue;
    std::set<std::string> processedFiles;
    std::string entryFile;
};

#endif // CHTL_PROCESSOR_H
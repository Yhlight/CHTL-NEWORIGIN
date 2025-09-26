#pragma once

#include "../CHTLJSNode/CHTLJSNode.h"
#include <string>
#include <vector>
#include <memory>

class CHTLJSGenerator {
public:
    std::string generate(const std::vector<std::unique_ptr<CHTLJSNode>>& ast);

private:
    std::string generateNode(const CHTLJSNode* node);
};
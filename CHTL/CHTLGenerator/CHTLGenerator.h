#pragma once

#include "../CHTLNode/BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

    class CHTLGenerator {
    public:
        std::string generate(const std::shared_ptr<BaseNode>& node);

    private:
        void generateNode(const std::shared_ptr<BaseNode>& node, std::string& output);
    };

}
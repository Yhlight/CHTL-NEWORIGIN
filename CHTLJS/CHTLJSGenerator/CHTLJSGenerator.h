#pragma once

#include "../CHTLJSNode/BaseNode.h"
#include <string>
#include <memory>
#include <sstream>

namespace CHTLJS {

    class CHTLJSGenerator {
    public:
        std::string generate(const std::shared_ptr<BaseNode>& node);

    private:
        void visit(const std::shared_ptr<BaseNode>& node);

        std::stringstream out;
    };

}
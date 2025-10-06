#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTLJS {

    class ScriptLoaderNode : public BaseNode {
    public:
        std::vector<std::string> scripts;

        ScriptLoaderNode() : BaseNode(NodeType::NODE_SCRIPT_LOADER) {}
    };

}
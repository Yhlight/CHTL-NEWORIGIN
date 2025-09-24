#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "UnifiedScanner.h"

namespace dispatcher {

struct DispatchResult {
    std::vector<std::string> chtl;
    std::vector<std::string> chtljs;
    std::vector<std::string> cssPlaceholders;
    std::vector<std::string> jsPlaceholders;
};

class CompilerDispatcher {
public:
    DispatchResult dispatch(std::string_view source);
};

}


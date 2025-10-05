#pragma once

#include <string>
#include <optional>

namespace CHTL {

    struct ElementTarget {
        std::string tagName;
        std::optional<int> index;
    };

}
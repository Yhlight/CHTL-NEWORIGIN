#pragma once
#include <string>

namespace chtljs {

class PlaceholderEncodingStrategy {
public:
    virtual ~PlaceholderEncodingStrategy() = default;
    virtual std::string encodeJs(const std::string& chunk) = 0;
};

}


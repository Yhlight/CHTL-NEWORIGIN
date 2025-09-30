#pragma once

#include <string>
#include <iostream>

namespace CHTL {

class CHTLIOStream {
public:
    CHTLIOStream();
    ~CHTLIOStream() = default;

    void write(const std::string& data);
    std::string read();
};

} // namespace CHTL
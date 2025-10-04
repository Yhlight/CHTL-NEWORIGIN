#pragma once

#include <string>

namespace CHTL {

class SaltBridge {
public:
    virtual ~SaltBridge() = default;

    // This method will be called by the CHTLGenerator to process a script block.
    // The CHTL JS compiler will provide the implementation.
    virtual std::string processScript(const std::string& raw_script) = 0;
};

}
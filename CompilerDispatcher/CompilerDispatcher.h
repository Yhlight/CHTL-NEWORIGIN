#pragma once

#include "Scanner/ScanResult.h"
#include <string>
#include <map>

namespace CHTL {

struct CompilationResult {
    std::string html_output;
    std::string css_output; // We can merge all CSS into one string
    std::string js_output; // We can merge all JS into one string
};

class CompilerDispatcher {
public:
    CompilationResult dispatch(const ScanResult& scanResult);
};

} // namespace CHTL

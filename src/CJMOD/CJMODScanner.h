#pragma once

#include "Arg.h"
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "../CHTL_JS/CHTLLexer/CHTLJSLexer.h" // For CHTLJSToken

// The result of a successful scan operation.
struct CJMODScanResult {
    std::unique_ptr<Arg> matched_arg;
    size_t start_index;
    size_t end_index;
};

// The CJMODScanner provides an interface for custom modules to request
// a scan of the source code to find values that match a predefined syntax pattern.
class CJMODScanner {
public:
    // Scans a token stream for a pattern defined by the 'syntax_arg', starting
    // the search from a given index.
    // The 'keyword' helps the scanner quickly locate potential matches.
    // Returns an optional ScanResult. If a match is found, the optional will
    // contain the result; otherwise, it will be empty.
    static std::optional<CJMODScanResult> scan(
        const std::vector<CHTLJSToken>& tokens,
        const Arg& syntax_arg,
        const std::string& keyword,
        size_t search_start_index
    );
};
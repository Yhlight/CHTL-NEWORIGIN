#pragma once

#include "Arg.h"
#include <string>
#include <memory>

// The CJMODScanner provides an interface for custom modules to request
// a scan of the source code to find values that match a predefined syntax pattern.
class CJMODScanner {
public:
    // Scans the source code for a pattern defined by the 'syntax_arg' and
    // returns a new Arg object containing the found values.
    // The 'keyword' helps the scanner quickly locate potential matches.
    static std::unique_ptr<Arg> scan(const Arg& syntax_arg, const std::string& keyword);
};
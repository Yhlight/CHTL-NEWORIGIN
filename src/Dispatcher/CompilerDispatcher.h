#pragma once

#include <string>
#include <vector>
#include "../Scanner/UnifiedScanner.h"

class CompilerDispatcher {
public:
    std::string compile(const std::string& source);

private:
    UnifiedScanner scanner;
};
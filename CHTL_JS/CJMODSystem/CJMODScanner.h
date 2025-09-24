#pragma once
#include <string>
#include <vector>

namespace cjmod {

struct ArgAtom {
    std::string token;
    std::string value;
};

struct Arg {
    std::vector<ArgAtom> atoms;
};

class CJMODScanner {
public:
    // Per CHTL.md: front-cut and two-pointer scanning are CJMOD helpers.
    // The UnifiedScanner MUST NOT implement these directly.
    static Arg scan(const std::string& source, const std::string& keyword);
};

} // namespace cjmod


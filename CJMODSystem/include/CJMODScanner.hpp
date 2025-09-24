#pragma once

#include <string>

namespace chtl::cjmod {

struct Arg { /* placeholder for CJMOD API Arg */ };

class CJMODScanner {
public:
    static Arg scan(const Arg& args, const std::string& keyword);
};

}

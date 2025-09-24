#pragma once

#include <string>

namespace chtl::dispatcher {

class CompilerDispatcher {
public:
    std::string compileToHtml(const std::string& source);
};

}

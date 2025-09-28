#pragma once

#include <string>

class CHTLJSCompiler {
public:
    std::string compile(const std::string& scanned_source);

private:
    std::string compileSelectors(std::string source);
    std::string compileListenFunctions(std::string source);
};
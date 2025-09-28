#pragma once

#include <string>
#include <map>
#include <vector> // For parseAnimateBlock

class CHTLJSCompiler {
public:
    std::string compile(const std::string& scanned_source);

private:
    std::string compileSelectors(std::string source);
    std::string compileListenFunctions(std::string source);
    std::string compileAnimateFunctions(std::string source);
    std::string compileDelegateFunctions(std::string source);
    std::string compileRouterFunctions(std::string source);
    std::string compileScriptLoaderFunctions(std::string source);
};
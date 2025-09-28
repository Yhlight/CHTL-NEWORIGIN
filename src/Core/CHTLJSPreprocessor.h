#pragma once

#include <string>
#include <map>

class CHTLJSPreprocessor {
public:
    void process(std::string& source);

private:
    std::map<std::string, std::string> vir_map;

    void findDeclarations(std::string& source);
    void substituteAccesses(std::string& source);
};
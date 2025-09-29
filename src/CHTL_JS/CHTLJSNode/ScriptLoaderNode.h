#pragma once

#include "CHTLJSBaseNode.h"
#include <string>
#include <vector>

class ScriptLoaderNode : public CHTLJSBaseNode {
private:
    std::vector<std::string> files;

public:
    ScriptLoaderNode(const std::vector<std::string>& files_to_load) : files(files_to_load) {}

    const std::vector<std::string>& getFiles() const {
        return files;
    }

    virtual std::string toString() const override {
        std::string result = "ScriptLoaderNode(Files: ";
        for (const auto& file : files) {
            result += file + " ";
        }
        result += ")";
        return result;
    }
};
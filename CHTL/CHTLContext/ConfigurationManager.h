#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "../CHTLNode/ConfigurationNode.h"

namespace CHTL {

class ConfigurationManager {
public:
    ConfigurationManager();

    void update(const std::shared_ptr<ConfigurationNode>& configNode);

    std::string getKeyword(const std::string& key) const;
    bool isKeyword(const std::string& lexeme) const;

private:
    std::map<std::string, std::string> keywords;
    void initialize_defaults();
};

}
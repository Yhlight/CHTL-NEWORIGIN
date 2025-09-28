#pragma once

#include <string>
#include <vector>
#include <map>

class UnifiedScanner {
public:
    std::string scan(const std::string& source);
    const std::map<std::string, std::string>& getPlaceholders() const;

private:
    std::string addPlaceholder(const std::string& code);

    std::map<std::string, std::string> placeholder_map;
    int placeholder_count = 0;
};
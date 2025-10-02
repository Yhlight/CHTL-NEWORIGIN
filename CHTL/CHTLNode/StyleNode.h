#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <utility>
#include <sstream>

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    void addProperty(const std::string& key, const std::string& value) {
        properties.push_back({key, value});
    }

    std::string toString(int depth = 0) const override {
        std::stringstream ss;
        ss << std::string(depth * 2, ' ') << "/* Style Block: ";
        for (size_t i = 0; i < properties.size(); ++i) {
            ss << properties[i].first << ": " << properties[i].second << ";";
            if (i < properties.size() - 1) {
                ss << " ";
            }
        }
        ss << " */" << std::endl;
        return ss.str();
    }

private:
    std::vector<std::pair<std::string, std::string>> properties;
};

#endif //CHTL_STYLE_NODE_H
#pragma once

#include "BaseNode.h"
#include <string>

class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& type, const std::string& content, const std::string& name = "");

    virtual NodeType getType() const override;
    std::string getOriginType() const;
    std::string getContent() const;
    std::string getName() const;

    virtual std::unique_ptr<BaseNode> clone() const override;

private:
    std::string originType;
    std::string content;
    std::string name;
};
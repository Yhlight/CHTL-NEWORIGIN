#pragma once

#include <memory>
#include <string>
#include <vector>

namespace CHTL {

class CHTLParserContext;
class BaseNode;

// Struct to hold the components of a parsed condition
struct ParsedCondition {
    std::string selector;
    std::string property;
    std::string op;
    std::string value;
};

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode);

// New function to parse a condition string
ParsedCondition parseCondition(const std::string& conditionStr);

}
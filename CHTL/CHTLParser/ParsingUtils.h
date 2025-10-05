#pragma once

#include <memory>
#include <string>

namespace CHTL {

class CHTLParserContext;
class BaseNode;

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode);
std::string parse_property_value(CHTLParserContext* context);
std::shared_ptr<BaseNode> parseExpression(CHTLParserContext* context);

}
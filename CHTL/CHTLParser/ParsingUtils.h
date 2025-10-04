#pragma once

#include <memory>

namespace CHTL {

class CHTLParserContext;
class BaseNode;

void parseProperties(CHTLParserContext* context, std::shared_ptr<BaseNode> parentNode);

}
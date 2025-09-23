#include "CHTLContext.h"
#include "../CHTLNode/ConfigNode.h"

CHTLContext::CHTLContext() {
    activeConfig = std::make_shared<ConfigNode>();
}

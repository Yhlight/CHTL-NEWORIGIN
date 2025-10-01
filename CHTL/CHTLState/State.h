#ifndef CHTL_STATE_H
#define CHTL_STATE_H

#include <memory>

namespace CHTL {

class CHTLParser; // Forward declaration
class BaseNode;

class State {
public:
    virtual ~State() = default;
    virtual void handle(CHTLParser& parser) = 0;
};

} // namespace CHTL

#endif // CHTL_STATE_H
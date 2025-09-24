#ifndef CHTL_STATE_H
#define CHTL_STATE_H

namespace CHTL {

class Context;
class Parser; // Forward-declare Parser

class State {
public:
    virtual ~State() = default;

    // Handle now takes the parser so it can access the token stream
    virtual void Handle(Context* context, Parser& parser) = 0;
};

} // namespace CHTL

#endif // CHTL_STATE_H

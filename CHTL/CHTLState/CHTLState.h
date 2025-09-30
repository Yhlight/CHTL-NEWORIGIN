#ifndef CHTL_STATE_H
#define CHTL_STATE_H

namespace CHTL {

class CHTLContext; // Forward declaration

class CHTLState {
public:
    virtual ~CHTLState() = default;
    virtual void handle(CHTLContext* context) = 0;
};

} // namespace CHTL

#endif // CHTL_STATE_H
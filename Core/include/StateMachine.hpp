#pragma once

namespace chtl::core {

class IStateMachine {
public:
    virtual ~IStateMachine() = default;
    virtual void reset() = 0;
};

class IStrategy {
public:
    virtual ~IStrategy() = default;
};

}

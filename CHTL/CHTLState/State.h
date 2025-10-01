#pragma once

class Context;

class State {
public:
    virtual ~State() = default;
    virtual void handle(Context& context) = 0;
};
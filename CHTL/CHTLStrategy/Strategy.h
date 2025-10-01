#pragma once

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual void execute() = 0;
};
#pragma once

#include "CHTLJSBaseNode.h"
#include <string>

class EventBindingNode : public CHTLJSBaseNode {
private:
    std::string selector;
    std::string event_name;
    std::string callback;

public:
    EventBindingNode(const std::string& sel, const std::string& event, const std::string& cb)
        : selector(sel), event_name(event), callback(cb) {}

    const std::string& getSelector() const {
        return selector;
    }

    const std::string& getEventName() const {
        return event_name;
    }

    const std::string& getCallback() const {
        return callback;
    }

    virtual std::string toString() const override {
        return "EventBindingNode(Selector: " + selector + ", Event: " + event_name + ", Callback: " + callback + ")";
    }
};
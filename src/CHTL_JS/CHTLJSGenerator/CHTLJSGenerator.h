#pragma once

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <string>

// Forward declarations of node types to avoid circular dependencies
class ScriptLoaderNode;
class EnhancedSelectorNode;
class ListenNode;
class EventBindingNode;
class DelegateNode;
class AnimateNode;
class VirtualObjectNode;
class RouterNode;

class CHTLJSGenerator {
public:
    std::string generate(const CHTLJSBaseNode& node);

private:
    std::string generateScriptLoader(const ScriptLoaderNode& node);
    std::string generateEnhancedSelector(const EnhancedSelectorNode& node);
    std::string generateListen(const ListenNode& node);
    std::string generateEventBinding(const EventBindingNode& node);
    std::string generateDelegate(const DelegateNode& node);
    std::string generateAnimate(const AnimateNode& node);
    std::string generateVirtualObject(const VirtualObjectNode& node);
    std::string generateRouter(const RouterNode& node);
};
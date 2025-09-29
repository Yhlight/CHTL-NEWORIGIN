#include "CHTLJSGenerator.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include <sstream>
#include <stdexcept>

std::string CHTLJSGenerator::generate(const CHTLJSBaseNode& node) {
    if (const auto* scriptLoaderNode = dynamic_cast<const ScriptLoaderNode*>(&node)) {
        return generateScriptLoader(*scriptLoaderNode);
    }
    if (const auto* enhancedSelectorNode = dynamic_cast<const EnhancedSelectorNode*>(&node)) {
        return generateEnhancedSelector(*enhancedSelectorNode);
    }
    if (const auto* listenNode = dynamic_cast<const ListenNode*>(&node)) {
        return generateListen(*listenNode);
    }
    if (const auto* eventBindingNode = dynamic_cast<const EventBindingNode*>(&node)) {
        return generateEventBinding(*eventBindingNode);
    }
    if (const auto* delegateNode = dynamic_cast<const DelegateNode*>(&node)) {
        return generateDelegate(*delegateNode);
    }
    if (const auto* animateNode = dynamic_cast<const AnimateNode*>(&node)) {
        return generateAnimate(*animateNode);
    }
    if (const auto* virtualObjectNode = dynamic_cast<const VirtualObjectNode*>(&node)) {
        return generateVirtualObject(*virtualObjectNode);
    }
    if (const auto* routerNode = dynamic_cast<const RouterNode*>(&node)) {
        return generateRouter(*routerNode);
    }

    throw std::runtime_error("Unsupported CHTLJSNode type for generation.");
}

std::string CHTLJSGenerator::generateScriptLoader(const ScriptLoaderNode& node) {
    std::stringstream ss;
    ss << "Louder.require([";
    const auto& files = node.getFiles();
    for (size_t i = 0; i < files.size(); ++i) {
        ss << "'" << files[i] << "'";
        if (i < files.size() - 1) {
            ss << ", ";
        }
    }
    ss << "], function() {\n";
    ss << "    // All scripts have been loaded\n";
    ss << "});\n";
    return ss.str();
}

std::string CHTLJSGenerator::generateEnhancedSelector(const EnhancedSelectorNode& node) {
    std::stringstream ss;
    std::string selector = node.getSelector();
    size_t bracket_pos = selector.find('[');
    size_t end_bracket_pos = selector.find(']');

    if (bracket_pos != std::string::npos && end_bracket_pos != std::string::npos && end_bracket_pos > bracket_pos) {
        std::string base_selector = selector.substr(0, bracket_pos);
        std::string index = selector.substr(bracket_pos + 1, end_bracket_pos - bracket_pos - 1);
        ss << "document.querySelectorAll('" << base_selector << "')[" << index << "]." << node.getExpression() << ";\n";
    } else {
        ss << "document.querySelector('" << selector << "')." << node.getExpression() << ";\n";
    }
    return ss.str();
}

std::string CHTLJSGenerator::generateListen(const ListenNode& node) {
    if (node.getSelector().empty()) {
        std::stringstream ss;
        ss << "{\n";
        for (auto it = node.getEvents().begin(); it != node.getEvents().end(); ++it) {
            ss << "    " << it->first << ": " << it->second;
            if (std::next(it) != node.getEvents().end()) {
                ss << ",";
            }
            ss << "\n";
        }
        ss << "}";
        return ss.str();
    }

    std::stringstream ss;
    std::string selector_str = node.getSelector();
    for (const auto& pair : node.getEvents()) {
        ss << "document.querySelector(" << selector_str << ").addEventListener('" << pair.first << "', " << pair.second << ");\n";
    }
    return ss.str();
}

std::string CHTLJSGenerator::generateEventBinding(const EventBindingNode& node) {
    std::stringstream ss;
    ss << "document.querySelector(" << node.getSelector() << ").addEventListener('" << node.getEventName() << "', " << node.getCallback() << ");\n";
    return ss.str();
}

std::string CHTLJSGenerator::generateDelegate(const DelegateNode& node) {
    std::stringstream ss;
    const std::string& parent_selector = node.getParentSelector();
    const std::string& target_selector = node.getTargetSelector();

    for (const auto& pair : node.getEvents()) {
        const std::string& event_name = pair.first;
        const std::string& callback = pair.second;
        ss << "document.querySelector(" << parent_selector << ").addEventListener('" << event_name << "', (event) => {\n";
        ss << "    if (event.target.matches(" << target_selector << ")) {\n";
        ss << "        (" << callback << ").call(event.target, event);\n";
        ss << "    }\n";
        ss << "});\n";
    }
    return ss.str();
}

std::string CHTLJSGenerator::generateAnimate(const AnimateNode& node) {
    std::stringstream ss;
    ss << "new Animate({\n";

    const auto& props = node.getProperties();
    for (auto it = props.begin(); it != props.end(); ++it) {
        ss << it->first << ": ";
        if (it->first == "target") {
            std::string target_selector = it->second;
            size_t start = target_selector.find("{{");
            size_t end = target_selector.find("}}");
            if (start != std::string::npos && end != std::string::npos) {
                target_selector = target_selector.substr(start + 2, end - start - 2);
            }
            ss << "document.querySelector(" << target_selector << ")";
        } else {
            ss << it->second;
        }
        ss << ",\n";
    }

    ss << "});\n";
    return ss.str();
}

std::string CHTLJSGenerator::generateVirtualObject(const VirtualObjectNode& node) {
    std::stringstream ss;
    ss << "const " << node.getName() << " = ";
    if (node.getAssignedBlock()) {
        ss << generate(*node.getAssignedBlock());
    } else {
        ss << "{}";
    }
    ss << ";\n";
    return ss.str();
}

std::string CHTLJSGenerator::generateRouter(const RouterNode& node) {
    std::stringstream ss;
    for (const auto& pair : node.getRoutes()) {
        std::string page_selector = pair.second;
        size_t start = page_selector.find("{{");
        size_t end = page_selector.find("}}");
        if (start != std::string::npos && end != std::string::npos) {
            page_selector = page_selector.substr(start + 2, end - start - 2);
        }
        ss << "Router.add('" << pair.first << "', document.querySelector(" << page_selector << "));\n";
    }
    return ss.str();
}
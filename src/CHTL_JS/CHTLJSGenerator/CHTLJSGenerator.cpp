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

// Helper to escape single quotes for JavaScript strings
std::string escapeJavaScriptString(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c == '\'') {
            result += "\\'";
        } else {
            result += c;
        }
    }
    return result;
}

std::string CHTLJSGenerator::generateSelectorPart(const std::string& selector) {
    std::stringstream ss;
    std::string escaped_selector = escapeJavaScriptString(selector);

    size_t bracket_pos = selector.find('[');
    bool has_indexer = (bracket_pos != std::string::npos && selector.back() == ']');
    bool is_id_selector = (selector.rfind('#', 0) == 0);

    if (is_id_selector || has_indexer) {
        if (has_indexer) {
            std::string base_selector = selector.substr(0, bracket_pos);
            std::string index = selector.substr(bracket_pos + 1, selector.length() - bracket_pos - 2);
            ss << "document.querySelectorAll('" << escapeJavaScriptString(base_selector) << "')[" << index << "]";
        } else {
            ss << "document.querySelector('" << escaped_selector << "')";
        }
    } else {
        ss << "document.querySelectorAll('" << escaped_selector << "')";
    }
    return ss.str();
}

std::string CHTLJSGenerator::generateEnhancedSelector(const EnhancedSelectorNode& node) {
    std::stringstream ss;
    ss << generateSelectorPart(node.getSelector());
    if (!node.getExpression().empty()) {
        ss << "." << node.getExpression();
    }
    ss << ";\n";
    return ss.str();
}

std::string CHTLJSGenerator::generateListen(const ListenNode& node) {
    // If the selector is empty, this is a Vir object definition, not an event listener.
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
    std::string selector_js = generateSelectorPart(node.getSelector());

    // Check if the selector implies a single element or a collection
    size_t bracket_pos = node.getSelector().find('[');
    bool is_single_element = (node.getSelector().rfind('#', 0) == 0 || (bracket_pos != std::string::npos && node.getSelector().back() == ']'));

    if (is_single_element) {
        for (const auto& pair : node.getEvents()) {
            ss << selector_js << ".addEventListener('" << pair.first << "', " << pair.second << ");\n";
        }
    } else {
        // If it's a collection, loop through and add the listener to each element.
        ss << selector_js << ".forEach(element => {\n";
        for (const auto& pair : node.getEvents()) {
            ss << "    element.addEventListener('" << pair.first << "', " << pair.second << ");\n";
        }
        ss << "});\n";
    }

    return ss.str();
}

std::string CHTLJSGenerator::generateEventBinding(const EventBindingNode& node) {
    std::stringstream ss;
    std::string selector_js = generateSelectorPart(node.getSelector());

    // Check if the selector implies a single element or a collection
    size_t bracket_pos = node.getSelector().find('[');
    bool is_single_element = (node.getSelector().rfind('#', 0) == 0 || (bracket_pos != std::string::npos && node.getSelector().back() == ']'));

    if (is_single_element) {
        ss << selector_js << ".addEventListener('" << node.getEventName() << "', " << node.getCallback() << ");\n";
    } else {
        // If it's a collection, loop through and add the listener to each element.
        ss << selector_js << ".forEach(element => {\n";
        ss << "    element.addEventListener('" << node.getEventName() << "', " << node.getCallback() << ");\n";
        ss << "});\n";
    }

    return ss.str();
}

std::string CHTLJSGenerator::generateDelegate(const DelegateNode& node) {
    std::stringstream ss;

    // Generate the JS for the parent element selector
    std::string parent_js = generateSelectorPart(node.getParentSelector());

    // For the target selector, we just need the raw string for the .matches() method
    std::string target_selector_raw = node.getTargetSelector();
    size_t start_pos = target_selector_raw.find("{{");
    size_t end_pos = target_selector_raw.rfind("}}");
    if (start_pos != std::string::npos && end_pos != std::string::npos) {
        target_selector_raw = target_selector_raw.substr(start_pos + 2, end_pos - (start_pos + 2));
    }

    for (const auto& pair : node.getEvents()) {
        const std::string& event_name = pair.first;
        const std::string& callback = pair.second;
        ss << parent_js << ".addEventListener('" << event_name << "', (event) => {\n";
        ss << "    if (event.target.matches('" << escapeJavaScriptString(target_selector_raw) << "')) {\n";
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
        ss << "    " << it->first << ": ";

        if (it->first == "target") {
            std::string target_value = it->second;
            // Check if the target is an enhanced selector
            size_t start_pos = target_value.find("{{");
            size_t end_pos = target_value.rfind("}}");
            if (start_pos != std::string::npos && end_pos != std::string::npos) {
                std::string selector = target_value.substr(start_pos + 2, end_pos - (start_pos + 2));
                ss << generateSelectorPart(selector);
            } else {
                // Otherwise, assume it's a variable or some other JS expression
                ss << target_value;
            }
        } else {
            // For all other properties, assume the value is a valid JS literal/expression
            ss << it->second;
        }

        // Add a comma if it's not the last element
        if (std::next(it) != props.end()) {
            ss << ",";
        }
        ss << "\n";
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
        std::string page_selector_value = pair.second;

        // By default, assume the value is a variable or expression
        std::string page_js = page_selector_value;

        // Check if it's an enhanced selector and generate the correct JS if so
        size_t start_pos = page_selector_value.find("{{");
        size_t end_pos = page_selector_value.rfind("}}");
        if (start_pos != std::string::npos && end_pos != std::string::npos) {
            std::string selector = page_selector_value.substr(start_pos + 2, end_pos - (start_pos + 2));
            page_js = generateSelectorPart(selector);
        }

        ss << "Router.add('" << pair.first << "', " << page_js << ");\n";
    }
    return ss.str();
}
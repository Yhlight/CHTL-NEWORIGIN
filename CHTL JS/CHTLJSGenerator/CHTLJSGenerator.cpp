#include "CHTLJSGenerator.h"
#include <iostream>
#include <sstream>

CHTLJSGenerator::CHTLJSGenerator() 
    : debugMode_(false)
    , indentSize_(2)
    , minify_(false)
    , currentIndent_(0) {
}

bool CHTLJSGenerator::generate(const std::unique_ptr<CHTLJSNode>& ast, std::string& output) {
    if (!ast) {
        reportError("AST为空");
        return false;
    }
    
    output_.clear();
    currentIndent_ = 0;
    
    if (debugMode_) {
        std::cout << "开始生成JavaScript代码..." << std::endl;
    }
    
    generateNode(*ast);
    output = output_;
    
    if (debugMode_) {
        std::cout << "JavaScript代码生成完成，长度: " << output.length() << std::endl;
    }
    
    return true;
}

void CHTLJSGenerator::generateNode(const CHTLJSNode& node) {
    switch (node.getType()) {
        case CHTLJSNodeType::ROOT:
            // 根节点，生成所有子节点
            for (const auto& child : node.getChildren()) {
                generateNode(*child);
            }
            break;
            
        case CHTLJSNodeType::LISTEN:
            generateListen(static_cast<const ListenNode&>(node));
            break;
            
        case CHTLJSNodeType::ANIMATE:
            generateAnimate(static_cast<const AnimateNode&>(node));
            break;
            
        case CHTLJSNodeType::ROUTER:
            generateRouter(static_cast<const RouterNode&>(node));
            break;
            
        case CHTLJSNodeType::SELECTOR:
            generateSelector(static_cast<const SelectorNode&>(node));
            break;
            
        case CHTLJSNodeType::EVENT_BINDING:
            generateEventBinding(static_cast<const EventBindingNode&>(node));
            break;
            
        case CHTLJSNodeType::ANIMATION:
            generateAnimation(static_cast<const AnimationNode&>(node));
            break;
            
        case CHTLJSNodeType::ROUTE:
            generateRoute(static_cast<const RouteNode&>(node));
            break;
            
        case CHTLJSNodeType::VIRTUAL_OBJECT:
            generateVirtualObject(static_cast<const VirtualObjectNode&>(node));
            break;
            
        default:
            if (debugMode_) {
                std::cout << "未知CHTL JS节点类型: " << static_cast<int>(node.getType()) << std::endl;
            }
            break;
    }
}

void CHTLJSGenerator::generateListen(const ListenNode& node) {
    addIndent();
    addText("// Listen事件绑定");
    addLine();
    
    for (const auto& child : node.getChildren()) {
        if (child->getType() == CHTLJSNodeType::EVENT_BINDING) {
            const EventBindingNode& eventBinding = static_cast<const EventBindingNode&>(*child);
            std::string eventJS = generateEventJS(eventBinding.getEventType(), eventBinding.getHandler());
            addText(eventJS);
            addLine();
        }
    }
}

void CHTLJSGenerator::generateAnimate(const AnimateNode& node) {
    addIndent();
    addText("// Animate动画");
    addLine();
    
    std::string animationJS = generateAnimationJS(node);
    addText(animationJS);
    addLine();
}

void CHTLJSGenerator::generateRouter(const RouterNode& node) {
    addIndent();
    addText("// Router路由");
    addLine();
    
    std::string routerJS = generateRouterJS(node);
    addText(routerJS);
    addLine();
}

void CHTLJSGenerator::generateSelector(const SelectorNode& node) {
    std::string selectorJS = generateSelectorJS(node.getSelector());
    addText(selectorJS);
}

void CHTLJSGenerator::generateEventBinding(const EventBindingNode& node) {
    std::string eventJS = generateEventJS(node.getEventType(), node.getHandler());
    addText(eventJS);
}

void CHTLJSGenerator::generateAnimation(const AnimationNode& node) {
    addIndent();
    addText("// Animation动画");
    addLine();
}

void CHTLJSGenerator::generateRoute(const RouteNode& node) {
    addIndent();
    addText("// Route: " + node.getUrl() + " -> " + node.getPage());
    addLine();
}

void CHTLJSGenerator::generateVirtualObject(const VirtualObjectNode& node) {
    addIndent();
    addText("// Virtual Object: " + node.getName());
    addLine();
}

void CHTLJSGenerator::addIndent() {
    if (!minify_) {
        for (int i = 0; i < currentIndent_ * indentSize_; i++) {
            output_ += " ";
        }
    }
}

void CHTLJSGenerator::addLine() {
    if (!minify_) {
        output_ += "\n";
    }
}

void CHTLJSGenerator::addText(const std::string& text) {
    output_ += text;
}

std::string CHTLJSGenerator::escapeJS(const std::string& text) {
    std::string result = text;
    
    // 转义JavaScript特殊字符
    size_t pos = 0;
    while ((pos = result.find("\\", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\'", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\'");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\n", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\r", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\r");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\t", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\t");
        pos += 2;
    }
    
    return result;
}

std::string CHTLJSGenerator::generateSelectorJS(const std::string& selector) {
    // 将CHTL JS选择器转换为JavaScript选择器
    std::string result = selector;
    
    // 移除{{}}包装
    if (result.length() >= 4 && result.substr(0, 2) == "{{" && result.substr(result.length() - 2) == "}}") {
        result = result.substr(2, result.length() - 4);
    }
    
    // 转换选择器语法
    if (result[0] == '.') {
        // 类选择器
        result = "document.querySelector('" + result + "')";
    } else if (result[0] == '#') {
        // ID选择器
        result = "document.getElementById('" + result.substr(1) + "')";
    } else {
        // 标签选择器
        result = "document.querySelector('" + result + "')";
    }
    
    return result;
}

std::string CHTLJSGenerator::generateEventJS(const std::string& eventType, const std::string& handler) {
    std::stringstream ss;
    ss << "element.addEventListener('" << eventType << "', " << handler << ");";
    return ss.str();
}

std::string CHTLJSGenerator::generateAnimationJS(const AnimateNode& node) {
    std::stringstream ss;
    ss << "const animation = {";
    
    if (!minify_) {
        ss << "\n";
        currentIndent_++;
        addIndent();
    }
    
    ss << "target: " << node.getAttribute("target") << ",";
    
    if (!minify_) {
        ss << "\n";
        addIndent();
    }
    
    ss << "duration: " << node.getAttribute("duration") << ",";
    
    if (!minify_) {
        ss << "\n";
        addIndent();
    }
    
    ss << "easing: '" << node.getAttribute("easing") << "'";
    
    if (!minify_) {
        currentIndent_--;
        ss << "\n";
        addIndent();
    }
    
    ss << "};";
    
    return ss.str();
}

std::string CHTLJSGenerator::generateRouterJS(const RouterNode& node) {
    std::stringstream ss;
    ss << "const router = {";
    
    if (!minify_) {
        ss << "\n";
        currentIndent_++;
        addIndent();
    }
    
    ss << "routes: [";
    
    for (const auto& child : node.getChildren()) {
        if (child->getType() == CHTLJSNodeType::ROUTE) {
            const RouteNode& route = static_cast<const RouteNode&>(*child);
            ss << "{ url: '" << route.getUrl() << "', page: " << route.getPage() << " }";
            
            if (child != node.getChildren().back()) {
                ss << ",";
            }
        }
    }
    
    ss << "]";
    
    if (!minify_) {
        currentIndent_--;
        ss << "\n";
        addIndent();
    }
    
    ss << "};";
    
    return ss.str();
}

void CHTLJSGenerator::reportError(const std::string& message) {
    std::cerr << "CHTL JS生成错误: " << message << std::endl;
    errors_.push_back(message);
}
#include "CHTLJSGenerator.h"
#include <sstream>
#include <iostream>

namespace CHTL {
namespace JS {

CHTLJSGenerator::CHTLJSGenerator(const JSGeneratorConfig& config)
    : config_(config), bridge_(Bridge::SaltBridge::getInstance()) {}

String CHTLJSGenerator::generate(const String& chtljsCode) {
    String result = chtljsCode;
    
    // 第零步：处理Vir虚对象（编译期语法糖，需要最先处理）
    result = processVirDeclarations(result);
    
    // 第一步：处理ScriptLoader（需要早期处理）
    result = processScriptLoaderBlocks(result);
    
    // 第二步：处理Router语法
    result = processRouterBlocks(result);
    
    // 第三步：处理Animate语法
    result = processAnimateBlocks(result);
    
    // 第四步：处理Listen语法（在{{}}处理之前）
    result = processListenBlocks(result);
    
    // 第五步：处理Delegate语法（在{{}}处理之前）
    result = processDelegateBlocks(result);
    
    // 第六步：处理事件绑定操作符 &->（在{{}}处理之前）
    result = processEventBindOperators(result);
    
    // 第六步：处理响应式值 $variable$
    size_t dollarPos = 0;
    while ((dollarPos = result.find("$", dollarPos)) != String::npos) {
        size_t endDollarPos = result.find("$", dollarPos + 1);
        if (endDollarPos != String::npos) {
            String varName = result.substr(dollarPos + 1, endDollarPos - dollarPos - 1);
            // 简单替换为变量引用（未来可以添加响应式系统）
            result.replace(dollarPos, endDollarPos - dollarPos + 1, varName);
            dollarPos += varName.length();
        } else {
            dollarPos++;
        }
    }
    
    // 第七步：处理增强选择器 {{...}}
    size_t pos = 0;
    while ((pos = result.find("{{", pos)) != String::npos) {
        size_t endPos = result.find("}}", pos);
        if (endPos == String::npos) {
            break;
        }
        
        String selector = result.substr(pos + 2, endPos - pos - 2);
        String jsCode = processEnhancedSelector(selector);
        result.replace(pos, endPos - pos + 2, jsCode);
        pos += jsCode.length();
    }
    
    // 第四步：处理 -> 操作符
    pos = 0;
    while ((pos = result.find("->", pos)) != String::npos) {
        result.replace(pos, 2, ".");
        pos += 1;
    }
    
    // 第五步：处理 & 引用（跳过 && 和 &->，以及位运算）
    pos = 0;
    while ((pos = result.find("&", pos)) != String::npos) {
        // 跳过 &&（逻辑运算符）
        if (pos + 1 < result.length() && result[pos + 1] == '&') {
            pos += 2;
            continue;
        }
        
        // 跳过 &->（事件绑定操作符，应该已被处理）
        if (pos + 2 < result.length() && result[pos + 1] == '-' && result[pos + 2] == '>') {
            pos += 3;
            continue;
        }
        
        // 跳过位运算符 &（前后都是数字/变量名/二进制字面量）
        bool isBitwiseOp = false;
        if (pos > 0 && pos + 1 < result.length()) {
            // 检查前面是否是数字、变量名、0b、0x、0o或右括号
            size_t before = pos - 1;
            while (before > 0 && std::isspace(result[before])) before--;
            
            char prevChar = result[before];
            bool hasValidBefore = std::isdigit(prevChar) || std::isalpha(prevChar) || 
                                  prevChar == '_' || prevChar == ')' || prevChar == ']';
            
            // 检查后面是否是数字、变量名、0b、0x、0o或左括号
            size_t after = pos + 1;
            while (after < result.length() && std::isspace(result[after])) after++;
            
            if (after < result.length()) {
                char nextChar = result[after];
                bool hasValidAfter = std::isdigit(nextChar) || std::isalpha(nextChar) || 
                                     nextChar == '_' || nextChar == '(' || nextChar == '[';
                
                // 如果前后都是有效的，可能是位运算
                if (hasValidBefore && hasValidAfter) {
                    isBitwiseOp = true;
                }
            }
        }
        
        if (isBitwiseOp) {
            pos++;
            continue;
        }
        
        String contextRef = bridge_.resolveAmpersand(false);
        if (!contextRef.empty()) {
            String jsRef = bridge_.convertEnhancedSelector(contextRef);
            result.replace(pos, 1, jsRef);
            pos += jsRef.length();
        } else {
            result.replace(pos, 1, "this");
            pos += 4;
        }
    }
    
    // 第六步：如果配置要求，用IIFE包装
    if (config_.wrapIIFE) {
        result = wrapWithIIFE(result);
    }
    
    return result;
}

String CHTLJSGenerator::processEnhancedSelector(const String& selector) {
    return bridge_.convertEnhancedSelector(selector);
}

String CHTLJSGenerator::processListen(const String& target, const String& events) {
    std::stringstream ss;
    
    // 简化实现：生成addEventListener调用
    ss << target << ".addEventListener(" << events << ")";
    
    return ss.str();
}

String CHTLJSGenerator::processEventBind(const String& target, const String& event, const String& handler) {
    std::stringstream ss;
    
    ss << target << ".addEventListener('" << event << "', " << handler << ")";
    
    return ss.str();
}

String CHTLJSGenerator::processDelegate(const String& parent, const String& /* config */) {
    std::stringstream ss;
    
    // 事件委托实现（简化版）
    ss << "// Event delegation for " << parent << "\n";
    
    return ss.str();
}

String CHTLJSGenerator::processDelegateBlocks(const String& code) {
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    // 创建解析器
    CHTLJSParser parser(code);
    
    // 查找所有Delegate块
    while (true) {
        auto blockPos = parser.findDelegateBlock(code, searchPos);
        if (!blockPos.has_value()) {
            // 没有更多Delegate块
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = blockPos->first;
        size_t endPos = blockPos->second;
        
        // 添加Delegate块之前的内容
        result += code.substr(lastPos, startPos - lastPos);
        
        // 提取并解析Delegate块
        String blockCode = code.substr(startPos, endPos - startPos);
        auto delegateBlock = parser.parseDelegateBlock(blockCode);
        
        if (delegateBlock.has_value()) {
            // 生成事件委托代码
            String parent = delegateBlock->parent;
            
            // 如果parent是{{...}}，转换它
            if (parent.length() >= 4 && parent.substr(0, 2) == "{{" && 
                parent.substr(parent.length() - 2) == "}}") {
                String selector = parent.substr(2, parent.length() - 4);
                parent = bridge_.convertEnhancedSelector(selector);
            }
            
            // 生成每个事件的委托
            std::stringstream ss;
            for (size_t i = 0; i < delegateBlock->eventBindings.size(); i++) {
                const auto& binding = delegateBlock->eventBindings[i];
                String eventName = binding.eventName;
                String handler = binding.handler;
                
                // 生成事件委托代码
                ss << parent << ".addEventListener('" << eventName << "', function(e) {\n";
                ss << "    // 事件委托：检查目标是否匹配\n";
                
                for (const auto& target : delegateBlock->targets) {
                    String targetSelector = target;
                    
                    // 转换{{...}}
                    if (targetSelector.length() >= 4 && targetSelector.substr(0, 2) == "{{" && 
                        targetSelector.substr(targetSelector.length() - 2) == "}}") {
                        String selector = targetSelector.substr(2, targetSelector.length() - 4);
                        
                        // 提取选择器字符串（用于matches）
                        if (selector.length() > 0 && selector[0] == '.') {
                            targetSelector = selector;
                        } else if (selector.length() > 0 && selector[0] == '#') {
                            targetSelector = selector;
                        } else {
                            targetSelector = selector;
                        }
                    }
                    
                    ss << "    if (e.target.matches('" << targetSelector << "') || e.target.closest('" << targetSelector << "')) {\n";
                    ss << "        (" << handler << ")(e);\n";
                    ss << "    }\n";
                }
                
                ss << "})";
                
                if (i < delegateBlock->eventBindings.size() - 1) {
                    ss << ";\n";
                }
            }
            
            result += ss.str();
        } else {
            // 解析失败，保留原始代码
            result += blockCode;
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

String CHTLJSGenerator::processAnimate(const String& /* config */) {
    std::stringstream ss;
    ss << "// Animation\n";
    return ss.str();
}

String CHTLJSGenerator::processAnimateBlocks(const String& code) {
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    CHTLJSParser parser(code);
    
    while (true) {
        auto blockPos = parser.findAnimateBlock(code, searchPos);
        if (!blockPos.has_value()) {
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = blockPos->first;
        size_t endPos = blockPos->second;
        
        result += code.substr(lastPos, startPos - lastPos);
        
        String blockCode = code.substr(startPos, endPos - startPos);
        auto animateBlock = parser.parseAnimateBlock(blockCode);
        
        if (animateBlock.has_value()) {
            // 生成动画代码
            std::stringstream ss;
            
            // 处理target
            Vector<String> jsTargets;
            for (const auto& target : animateBlock->targets) {
                String jsTarget = target;
                if (jsTarget.length() >= 4 && jsTarget.substr(0, 2) == "{{" && 
                    jsTarget.substr(jsTarget.length() - 2) == "}}") {
                    String selector = jsTarget.substr(2, jsTarget.length() - 4);
                    jsTarget = bridge_.convertEnhancedSelector(selector);
                }
                jsTargets.push_back(jsTarget);
            }
            
            // 生成动画函数IIFE
            ss << "(function() {\n";
            ss << "    const targets = [";
            for (size_t i = 0; i < jsTargets.size(); i++) {
                if (i > 0) ss << ", ";
                ss << jsTargets[i];
            }
            ss << "];\n";
            
            ss << "    const duration = " << animateBlock->duration << ";\n";
            ss << "    const startTime = Date.now();\n";
            
            // 起始状态
            if (!animateBlock->begin.empty()) {
                ss << "    targets.forEach(target => {\n";
                for (const auto& [prop, val] : animateBlock->begin) {
                    ss << "        target.style['" << prop << "'] = " << val << ";\n";
                }
                ss << "    });\n";
            }
            
            // 动画循环
            ss << "    function animate() {\n";
            ss << "        const elapsed = Date.now() - startTime;\n";
            ss << "        const progress = Math.min(elapsed / duration, 1);\n";
            
            // 处理关键帧
            if (!animateBlock->when.empty()) {
                ss << "        // 关键帧\n";
                for (const auto& kf : animateBlock->when) {
                    ss << "        if (progress >= " << kf.at << ") {\n";
                    ss << "            targets.forEach(target => {\n";
                    for (const auto& [prop, val] : kf.properties) {
                        ss << "                target.style['" << prop << "'] = " << val << ";\n";
                    }
                    ss << "            });\n";
                    ss << "        }\n";
                }
            }
            
            // 应用结束状态
            if (!animateBlock->end.empty()) {
                ss << "        if (progress >= 1) {\n";
                ss << "            targets.forEach(target => {\n";
                for (const auto& [prop, val] : animateBlock->end) {
                    ss << "                target.style['" << prop << "'] = " << val << ";\n";
                }
                ss << "            });\n";
                ss << "        }\n";
            }
            
            ss << "        if (progress < 1) {\n";
            ss << "            requestAnimationFrame(animate);\n";
            ss << "        }";
            
            if (!animateBlock->callback.empty()) {
                ss << " else {\n";
                ss << "            (" << animateBlock->callback << ")();\n";
                ss << "        }";
            }
            
            ss << "\n    }\n";
            ss << "    requestAnimationFrame(animate);\n";
            ss << "})()";
            
            result += ss.str();
        } else {
            result += blockCode;
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

String CHTLJSGenerator::processRouter(const String& /* config */) {
    std::stringstream ss;
    ss << "// Router\n";
    return ss.str();
}

String CHTLJSGenerator::processRouterBlocks(const String& code) {
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    CHTLJSParser parser(code);
    
    while (true) {
        auto blockPos = parser.findRouterBlock(code, searchPos);
        if (!blockPos.has_value()) {
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = blockPos->first;
        size_t endPos = blockPos->second;
        
        result += code.substr(lastPos, startPos - lastPos);
        
        String blockCode = code.substr(startPos, endPos - startPos);
        auto routerBlock = parser.parseRouterBlock(blockCode);
        
        if (routerBlock.has_value()) {
            std::stringstream ss;
            
            ss << "(function() {\n";
            // 设置路由器配置
            ss << "    const config = {\n";
            ss << "        mode: '" << routerBlock->mode << "',\n";
            if (!routerBlock->rootPath.empty()) {
                ss << "        root: '" << routerBlock->rootPath << "'\n";
            } else {
                ss << "        root: '/'\n";
            }
            ss << "    };\n";
            ss << "    const routes = {};\n";
            
            for (const auto& route : routerBlock->routes) {
                String page = route.page;
                if (page.length() >= 4 && page.substr(0, 2) == "{{" && 
                    page.substr(page.length() - 2) == "}}") {
                    String selector = page.substr(2, page.length() - 4);
                    page = bridge_.convertEnhancedSelector(selector);
                }
                ss << "    routes['" << route.url << "'] = " << page << ";\n";
            }
            
            ss << "    \n";
            ss << "    function navigate(path) {\n";
            ss << "        Object.values(routes).forEach(el => {\n";
            ss << "            if (el) el.style.display = 'none';\n";
            ss << "        });\n";
            ss << "        const target = routes[path];\n";
            ss << "        if (target) target.style.display = '';\n";
            ss << "    }\n";
            ss << "    \n";
            
            if (routerBlock->mode == "history") {
                ss << "    function getPath() {\n";
                ss << "        let path = window.location.pathname;\n";
                ss << "        if (config.root !== '/') {\n";
                ss << "            path = path.replace(config.root, '');\n";
                ss << "        }\n";
                ss << "        return path || '/';\n";
                ss << "    }\n";
                ss << "    window.addEventListener('popstate', () => {\n";
                ss << "        navigate(getPath());\n";
                ss << "    });\n";
                ss << "    navigate(getPath());\n";
            } else {
                ss << "    window.addEventListener('hashchange', () => {\n";
                ss << "        const path = window.location.hash.slice(1) || '/';\n";
                ss << "        navigate(path);\n";
                ss << "    });\n";
                ss << "    const initialPath = window.location.hash.slice(1) || '/';\n";
                ss << "    navigate(initialPath);\n";
            }
            
            ss << "})()";
            
            result += ss.str();
        } else {
            result += blockCode;
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

String CHTLJSGenerator::processVirDeclarations(const String& code) {
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    CHTLJSParser parser(code);
    
    while (true) {
        auto virPos = parser.findVirDeclaration(code, searchPos);
        if (!virPos.has_value()) {
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = virPos->first;
        size_t endPos = virPos->second;
        
        result += code.substr(lastPos, startPos - lastPos);
        
        String virCode = code.substr(startPos, endPos - startPos);
        auto virObj = parser.parseVirDeclaration(virCode);
        
        if (virObj.has_value()) {
            // 注册到全局 VirRegistry
            VirObjectInfo virInfo(virObj->name, virObj->type, virCode);
            virInfo.hasVirtualObject = virObj->hasVirtualObject;
            virInfo.virtualKeys = virObj->virtualKeys;
            VirRegistry::getInstance().registerVir(virObj->name, virInfo);
            
            // Vir是编译期语法糖，生成时保留原始代码（去掉Vir关键字）
            // Vir test = Listen {...}; → const test = Listen {...};
            String generated = "const " + virCode.substr(4);  // 去掉"Vir "
            result += generated;
        } else {
            result += virCode;
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

String CHTLJSGenerator::processScriptLoaderBlocks(const String& code) {
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    CHTLJSParser parser(code);
    
    while (true) {
        auto loaderPos = parser.findScriptLoaderBlock(code, searchPos);
        if (!loaderPos.has_value()) {
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = loaderPos->first;
        size_t endPos = loaderPos->second;
        
        result += code.substr(lastPos, startPos - lastPos);
        
        String loaderCode = code.substr(startPos, endPos - startPos);
        auto loader = parser.parseScriptLoaderBlock(loaderCode);
        
        if (loader.has_value()) {
            std::stringstream ss;
            
            ss << "(function() {\n";
            ss << "    const scripts = " << "[";
            for (size_t i = 0; i < loader->scripts.size(); i++) {
                if (i > 0) ss << ", ";
                ss << "'" << loader->scripts[i] << "'";
            }
            ss << "];\n";
            ss << "    let loaded = 0;\n";
            ss << "    scripts.forEach(src => {\n";
            ss << "        const script = document.createElement('script');\n";
            ss << "        script.src = src;\n";
            if (loader->async) {
                ss << "        script.async = true;\n";
            }
            ss << "        script.onload = () => {\n";
            ss << "            loaded++;\n";
            ss << "            if (loaded === scripts.length) {\n";
            if (!loader->onload.empty()) {
                ss << "                (" << loader->onload << ")();\n";
            }
            ss << "            }\n";
            ss << "        };\n";
            if (!loader->onerror.empty()) {
                ss << "        script.onerror = " << loader->onerror << ";\n";
            }
            ss << "        document.head.appendChild(script);\n";
            ss << "    });\n";
            ss << "})()";
            
            result += ss.str();
        } else {
            result += loaderCode;
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

String CHTLJSGenerator::wrapWithIIFE(const String& code) {
    std::stringstream ss;
    
    if (config_.useStrict) {
        ss << "(function() {\n";
        ss << "  'use strict';\n";
        ss << "  " << code << "\n";
        ss << "})();\n";
    } else {
        ss << "(function() {\n";
        ss << "  " << code << "\n";
        ss << "})();\n";
    }
    
    return ss.str();
}

String CHTLJSGenerator::processListenBlocks(const String& code) {
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    // 创建解析器
    CHTLJSParser parser(code);
    
    // 查找所有Listen块
    while (true) {
        auto blockPos = parser.findListenBlock(code, searchPos);
        if (!blockPos.has_value()) {
            // 没有更多Listen块，添加剩余内容
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = blockPos->first;
        size_t endPos = blockPos->second;
        
        // 添加Listen块之前的内容
        result += code.substr(lastPos, startPos - lastPos);
        
        // 提取并解析Listen块
        String blockCode = code.substr(startPos, endPos - startPos);
        auto listenBlock = parser.parseListenBlock(blockCode);
        
        if (listenBlock.has_value()) {
            // 生成addEventListener代码
            String target = listenBlock->target;
            
            // 如果target是{{...}}，转换它
            if (target.length() >= 4 && target.substr(0, 2) == "{{" && 
                target.substr(target.length() - 2) == "}}") {
                String selector = target.substr(2, target.length() - 4);
                target = bridge_.convertEnhancedSelector(selector);
            }
            
            // 生成每个事件绑定
            std::stringstream ss;
            for (size_t i = 0; i < listenBlock->bindings.size(); i++) {
                const auto& binding = listenBlock->bindings[i];
                
                ss << target << ".addEventListener('" 
                   << binding.eventName << "', " 
                   << binding.handler << ")";
                
                // 添加分号和换行（除了最后一个）
                if (i < listenBlock->bindings.size() - 1) {
                    ss << ";\n    ";
                }
            }
            
            result += ss.str();
        } else {
            // 解析失败，保留原始代码
            result += blockCode;
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

String CHTLJSGenerator::processEventBindOperators(const String& code) {
    String result;
    size_t lastPos = 0;
    size_t searchPos = 0;
    
    // 创建解析器
    CHTLJSParser parser(code);
    
    // 查找所有&->操作符
    while (true) {
        auto opPos = parser.findEventBindOperator(code, searchPos);
        if (!opPos.has_value()) {
            // 没有更多&->操作符，添加剩余内容
            result += code.substr(lastPos);
            break;
        }
        
        size_t startPos = opPos->first;
        size_t endPos = opPos->second;
        
        // 添加&->之前的内容
        result += code.substr(lastPos, startPos - lastPos);
        
        // 提取并解析&->操作符
        String opCode = code.substr(startPos, endPos - startPos);
        
        // 检查是否是链式绑定（包含多个&->）
        size_t count = 0;
        size_t checkPos = 0;
        while ((checkPos = opCode.find("&->", checkPos)) != String::npos) {
            count++;
            checkPos += 3;
        }
        
        if (count > 1) {
            // 链式绑定，使用parseChainBinding
            Vector<EventBindOperator> chainOps = parser.parseChainBinding(opCode);
            
            std::stringstream ss;
            for (size_t i = 0; i < chainOps.size(); i++) {
                String target = chainOps[i].target;
                
                // 如果target是{{...}}，转换它
                if (target.length() >= 4 && target.substr(0, 2) == "{{" && 
                    target.substr(target.length() - 2) == "}}") {
                    String selector = target.substr(2, target.length() - 4);
                    target = bridge_.convertEnhancedSelector(selector);
                }
                
                // 生成每个事件的addEventListener
                for (const auto& eventName : chainOps[i].eventNames) {
                    ss << target << ".addEventListener('" 
                       << eventName << "', " 
                       << chainOps[i].handler << ")";
                    
                    if (i < chainOps.size() - 1 || chainOps[i].eventNames.size() > 1) {
                        ss << ";\n    ";
                    }
                }
            }
            
            result += ss.str();
        } else {
            // 单个&->，使用原有逻辑
            auto eventBindOp = parser.parseEventBindOperator(opCode);
            
            if (eventBindOp.has_value()) {
                // 生成addEventListener代码
                String target = eventBindOp->target;
            
            // 如果target是{{...}}，转换它
            if (target.length() >= 4 && target.substr(0, 2) == "{{" && 
                target.substr(target.length() - 2) == "}}") {
                String selector = target.substr(2, target.length() - 4);
                target = bridge_.convertEnhancedSelector(selector);
            }
            
            // 处理绑定块形式
            if (eventBindOp->isBlock) {
                // 重新解析块内容获取所有绑定
                // 从&->之后查找{
                size_t arrowPos = opCode.find("&->");
                size_t blockStart = String::npos;
                if (arrowPos != String::npos) {
                    blockStart = opCode.find('{', arrowPos + 3);
                }
                if (blockStart != String::npos) {
                    String blockContent = parser.extractBlockContent(opCode, blockStart);
                    auto bindings = parser.parseEventBindings(blockContent);
                    
                    std::stringstream ss;
                    for (size_t i = 0; i < bindings.size(); i++) {
                        ss << target << ".addEventListener('" 
                           << bindings[i].eventName << "', " 
                           << bindings[i].handler << ")";
                        
                        if (i < bindings.size() - 1) {
                            ss << ";\n    ";
                        } else {
                            // 最后一个不加分号（会在后面统一处理）
                        }
                    }
                    result += ss.str();
                } else {
                    // 没有找到块，保留原始代码
                    result += opCode;
                }
            } else {
                // 单行或多事件形式
                std::stringstream ss;
                for (size_t i = 0; i < eventBindOp->eventNames.size(); i++) {
                    ss << target << ".addEventListener('" 
                       << eventBindOp->eventNames[i] << "', " 
                       << eventBindOp->handler << ")";
                    
                    if (i < eventBindOp->eventNames.size() - 1) {
                        ss << ";\n    ";
                    }
                }
                result += ss.str();
            }
            } else {
                // 解析失败，保留原始代码
                result += opCode;
            }
        }
        
        lastPos = endPos;
        searchPos = endPos;
    }
    
    return result;
}

} // namespace JS
} // namespace CHTL

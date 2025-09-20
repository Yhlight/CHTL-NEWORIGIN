#include "CHTLJSParser.h"
#include <iostream>
#include <sstream>

CHTLJSParser::CHTLJSParser() 
    : currentToken_(0)
    , debugMode_(false) {
}

bool CHTLJSParser::parse(const std::vector<CHTLJSToken>& tokens, std::unique_ptr<CHTLJSNode>& ast) {
    tokens_ = tokens;
    currentToken_ = 0;
    
    if (debugMode_) {
        std::cout << "开始CHTL JS语法分析，共 " << tokens.size() << " 个token" << std::endl;
    }
    
    return parseRoot(ast);
}

bool CHTLJSParser::parseRoot(std::unique_ptr<CHTLJSNode>& root) {
    root = std::make_unique<CHTLJSNode>(CHTLJSNodeType::ROOT);
    
    while (!isAtEnd()) {
        std::unique_ptr<CHTLJSNode> child;
        
        if (match(CHTLJSTokenType::LISTEN)) {
            if (!parseListen(child)) {
                return false;
            }
        } else if (match(CHTLJSTokenType::ANIMATE)) {
            if (!parseAnimate(child)) {
                return false;
            }
        } else if (match(CHTLJSTokenType::ROUTER)) {
            if (!parseRouter(child)) {
                return false;
            }
        } else if (match(CHTLJSTokenType::VIR)) {
            if (!parseVir(child)) {
                return false;
            }
        } else if (match(CHTLJSTokenType::SELECTOR)) {
            if (!parseSelector(child)) {
                return false;
            }
        } else {
            // 跳过未知token
            advance();
            continue;
        }
        
        if (child) {
            root->addChild(std::move(child));
        }
    }
    
    return true;
}

bool CHTLJSParser::parseListen(std::unique_ptr<CHTLJSNode>& listen) {
    Token listenToken = currentToken();
    advance(); // 跳过 Listen
    
    listen = std::make_unique<ListenNode>();
    
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            // 解析事件绑定
            std::unique_ptr<CHTLJSNode> eventBinding;
            if (!parseEventBinding(eventBinding)) {
                return false;
            }
            
            if (eventBinding) {
                listen->addChild(std::move(eventBinding));
            }
        }
        
        if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLJSParser::parseAnimate(std::unique_ptr<CHTLJSNode>& animate) {
    Token animateToken = currentToken();
    advance(); // 跳过 Animate
    
    animate = std::make_unique<AnimateNode>();
    
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            // 解析动画属性
            if (match(CHTLJSTokenType::IDENTIFIER)) {
                Token attrName = currentToken();
                advance();
                
                if (match(CHTLJSTokenType::COLON)) {
                    advance(); // 跳过 :
                    
                    if (match({CHTLJSTokenType::STRING, CHTLJSTokenType::NUMBER, CHTLJSTokenType::BOOLEAN})) {
                        Token attrValue = currentToken();
                        animate->setAttribute(attrName.value, attrValue.value);
                        advance();
                    } else {
                        reportError("期望属性值");
                        return false;
                    }
                } else {
                    reportError("期望 :");
                    return false;
                }
            } else {
                advance(); // 跳过其他token
            }
        }
        
        if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLJSParser::parseRouter(std::unique_ptr<CHTLJSNode>& router) {
    Token routerToken = currentToken();
    advance(); // 跳过 Router
    
    router = std::make_unique<RouterNode>();
    
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            // 解析路由
            std::unique_ptr<CHTLJSNode> route;
            if (!parseRoute(route)) {
                return false;
            }
            
            if (route) {
                router->addChild(std::move(route));
            }
        }
        
        if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLJSParser::parseVir(std::unique_ptr<CHTLJSNode>& vir) {
    Token virToken = currentToken();
    advance(); // 跳过 Vir
    
    if (!match(CHTLJSTokenType::IDENTIFIER)) {
        reportError("期望虚对象名称");
        return false;
    }
    
    Token nameToken = currentToken();
    advance(); // 跳过名称
    
    vir = std::make_unique<VirtualObjectNode>(nameToken.value);
    
    if (match(CHTLJSTokenType::EQUAL)) {
        advance(); // 跳过 =
        
        // 解析虚对象内容
        if (match(CHTLJSTokenType::LEFT_BRACE)) {
            advance(); // 跳过 {
            
            while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
                // 解析虚对象属性
                if (match(CHTLJSTokenType::IDENTIFIER)) {
                    Token attrName = currentToken();
                    advance();
                    
                    if (match(CHTLJSTokenType::COLON)) {
                        advance(); // 跳过 :
                        
                        if (match({CHTLJSTokenType::STRING, CHTLJSTokenType::NUMBER, CHTLJSTokenType::BOOLEAN})) {
                            Token attrValue = currentToken();
                            vir->setAttribute(attrName.value, attrValue.value);
                            advance();
                        } else {
                            reportError("期望属性值");
                            return false;
                        }
                    } else {
                        reportError("期望 :");
                        return false;
                    }
                } else {
                    advance(); // 跳过其他token
                }
            }
            
            if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
                reportError("期望 }");
                return false;
            }
            advance(); // 跳过 }
        }
    }
    
    return true;
}

bool CHTLJSParser::parseSelector(std::unique_ptr<CHTLJSNode>& selector) {
    Token selectorToken = currentToken();
    advance(); // 跳过选择器token
    
    selector = std::make_unique<SelectorNode>(selectorToken.value);
    return true;
}

bool CHTLJSParser::parseEventBinding(std::unique_ptr<CHTLJSNode>& eventBinding) {
    if (!match(CHTLJSTokenType::IDENTIFIER)) {
        return false;
    }
    
    Token eventType = currentToken();
    advance(); // 跳过事件类型
    
    if (!match(CHTLJSTokenType::COLON)) {
        reportError("期望 :");
        return false;
    }
    advance(); // 跳过 :
    
    // 解析事件处理函数
    std::string handler;
    while (!isAtEnd() && !match(CHTLJSTokenType::COMMA) && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        handler += currentToken().value;
        advance();
    }
    
    eventBinding = std::make_unique<EventBindingNode>(eventType.value, handler);
    return true;
}

bool CHTLJSParser::parseAnimation(std::unique_ptr<CHTLJSNode>& animation) {
    animation = std::make_unique<AnimationNode>();
    return true;
}

bool CHTLJSParser::parseRoute(std::unique_ptr<CHTLJSNode>& route) {
    if (!match(CHTLJSTokenType::IDENTIFIER)) {
        return false;
    }
    
    Token urlToken = currentToken();
    advance(); // 跳过URL
    
    if (!match(CHTLJSTokenType::COLON)) {
        reportError("期望 :");
        return false;
    }
    advance(); // 跳过 :
    
    if (!match({CHTLJSTokenType::STRING, CHTLJSTokenType::SELECTOR})) {
        reportError("期望页面选择器");
        return false;
    }
    
    Token pageToken = currentToken();
    advance(); // 跳过页面
    
    route = std::make_unique<RouteNode>(urlToken.value, pageToken.value);
    return true;
}

bool CHTLJSParser::parseVirtualObject(std::unique_ptr<CHTLJSNode>& virtualObject) {
    if (!match(CHTLJSTokenType::IDENTIFIER)) {
        return false;
    }
    
    Token nameToken = currentToken();
    advance(); // 跳过名称
    
    virtualObject = std::make_unique<VirtualObjectNode>(nameToken.value);
    return true;
}

CHTLJSToken CHTLJSParser::currentToken() const {
    if (isAtEnd()) {
        return CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens_[currentToken_];
}

CHTLJSToken CHTLJSParser::peekToken(size_t offset) const {
    if (currentToken_ + offset >= tokens_.size()) {
        return CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens_[currentToken_ + offset];
}

bool CHTLJSParser::match(CHTLJSTokenType type) {
    return currentToken().type == type;
}

bool CHTLJSParser::match(const std::vector<CHTLJSTokenType>& types) {
    for (CHTLJSTokenType type : types) {
        if (currentToken().type == type) {
            return true;
        }
    }
    return false;
}

void CHTLJSParser::advance() {
    if (!isAtEnd()) {
        currentToken_++;
    }
}

bool CHTLJSParser::isAtEnd() const {
    return currentToken_ >= tokens_.size();
}

void CHTLJSParser::reportError(const std::string& message) {
    CHTLJSToken token = currentToken();
    reportError(message, token);
}

void CHTLJSParser::reportError(const std::string& message, const CHTLJSToken& token) {
    std::stringstream ss;
    ss << "CHTL JS解析错误 [" << token.line << ":" << token.column << "]: " << message;
    errors_.push_back(ss.str());
    std::cerr << ss.str() << std::endl;
}
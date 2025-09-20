#include "CHTLParser.h"
#include <iostream>
#include <sstream>

CHTLParser::CHTLParser() 
    : currentToken_(0)
    , debugMode_(false) {
}

bool CHTLParser::parse(const std::vector<Token>& tokens, std::unique_ptr<CHTLNode>& ast) {
    tokens_ = tokens;
    currentToken_ = 0;
    
    if (debugMode_) {
        std::cout << "开始语法分析，共 " << tokens.size() << " 个token" << std::endl;
    }
    
    return parseRoot(ast);
}

bool CHTLParser::parseRoot(std::unique_ptr<CHTLNode>& root) {
    root = std::make_unique<CHTLNode>(NodeType::ROOT);
    
    while (!isAtEnd()) {
        std::unique_ptr<CHTLNode> child;
        
        if (match(TokenType::TEMPLATE)) {
            if (!parseTemplate(child)) {
                return false;
            }
        } else if (match(TokenType::CUSTOM)) {
            if (!parseCustom(child)) {
                return false;
            }
        } else if (match(TokenType::ORIGIN)) {
            if (!parseOrigin(child)) {
                return false;
            }
        } else if (match(TokenType::IMPORT)) {
            if (!parseImport(child)) {
                return false;
            }
        } else if (match(TokenType::CONFIGURATION)) {
            if (!parseConfiguration(child)) {
                return false;
            }
        } else if (match(TokenType::NAMESPACE)) {
            if (!parseNamespace(child)) {
                return false;
            }
        } else if (match({TokenType::ELEMENT, TokenType::IDENTIFIER})) {
            if (!parseElement(child)) {
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

bool CHTLParser::parseElement(std::unique_ptr<CHTLNode>& element) {
    Token elementToken = currentToken();
    
    if (elementToken.type == TokenType::ELEMENT) {
        element = std::make_unique<ElementNode>(elementToken.value);
    } else if (elementToken.type == TokenType::IDENTIFIER) {
        element = std::make_unique<ElementNode>(elementToken.value);
    } else {
        reportError("期望元素名称", elementToken);
        return false;
    }
    
    advance(); // 跳过元素名称
    
    // 解析属性
    if (match(TokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        // 解析属性
        parseAttributes(element);
        
        // 解析子元素
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::unique_ptr<CHTLNode> child;
            
            if (match({TokenType::ELEMENT, TokenType::IDENTIFIER})) {
                if (!parseElement(child)) {
                    return false;
                }
            } else if (match(TokenType::TEXT)) {
                child = std::make_unique<TextNode>(currentToken().value);
                advance();
            } else if (match(TokenType::STYLE)) {
                if (!parseStyle(child)) {
                    return false;
                }
            } else if (match(TokenType::SCRIPT)) {
                if (!parseScript(child)) {
                    return false;
                }
            } else {
                advance(); // 跳过未知token
                continue;
            }
            
            if (child) {
                element->addChild(std::move(child));
            }
        }
        
        if (!match(TokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLParser::parseTemplate(std::unique_ptr<CHTLNode>& template_) {
    Token templateToken = currentToken();
    advance(); // 跳过 [Template]
    
    if (!match(TokenType::AT)) {
        reportError("期望 @ 符号");
        return false;
    }
    advance(); // 跳过 @
    
    Token typeToken = currentToken();
    if (typeToken.type != TokenType::IDENTIFIER) {
        reportError("期望模板类型");
        return false;
    }
    advance(); // 跳过类型
    
    Token nameToken = currentToken();
    if (nameToken.type != TokenType::IDENTIFIER) {
        reportError("期望模板名称");
        return false;
    }
    advance(); // 跳过名称
    
    template_ = std::make_unique<TemplateNode>(typeToken.value, nameToken.value);
    
    // 解析模板内容
    if (match(TokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::unique_ptr<CHTLNode> child;
            
            if (match({TokenType::ELEMENT, TokenType::IDENTIFIER})) {
                if (!parseElement(child)) {
                    return false;
                }
            } else {
                advance(); // 跳过未知token
                continue;
            }
            
            if (child) {
                template_->addChild(std::move(child));
            }
        }
        
        if (!match(TokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLParser::parseCustom(std::unique_ptr<CHTLNode>& custom) {
    Token customToken = currentToken();
    advance(); // 跳过 [Custom]
    
    if (!match(TokenType::AT)) {
        reportError("期望 @ 符号");
        return false;
    }
    advance(); // 跳过 @
    
    Token typeToken = currentToken();
    if (typeToken.type != TokenType::IDENTIFIER) {
        reportError("期望自定义类型");
        return false;
    }
    advance(); // 跳过类型
    
    Token nameToken = currentToken();
    if (nameToken.type != TokenType::IDENTIFIER) {
        reportError("期望自定义名称");
        return false;
    }
    advance(); // 跳过名称
    
    custom = std::make_unique<CustomNode>(typeToken.value, nameToken.value);
    
    // 解析自定义内容
    if (match(TokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::unique_ptr<CHTLNode> child;
            
            if (match({TokenType::ELEMENT, TokenType::IDENTIFIER})) {
                if (!parseElement(child)) {
                    return false;
                }
            } else {
                advance(); // 跳过未知token
                continue;
            }
            
            if (child) {
                custom->addChild(std::move(child));
            }
        }
        
        if (!match(TokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLParser::parseOrigin(std::unique_ptr<CHTLNode>& origin) {
    // 简化实现
    origin = std::make_unique<CHTLNode>(NodeType::ORIGIN);
    advance(); // 跳过 [Origin]
    return true;
}

bool CHTLParser::parseImport(std::unique_ptr<CHTLNode>& import) {
    // 简化实现
    import = std::make_unique<CHTLNode>(NodeType::IMPORT);
    advance(); // 跳过 [Import]
    return true;
}

bool CHTLParser::parseConfiguration(std::unique_ptr<CHTLNode>& config) {
    // 简化实现
    config = std::make_unique<CHTLNode>(NodeType::CONFIGURATION);
    advance(); // 跳过 [Configuration]
    return true;
}

bool CHTLParser::parseNamespace(std::unique_ptr<CHTLNode>& namespace_) {
    // 简化实现
    namespace_ = std::make_unique<CHTLNode>(NodeType::NAMESPACE);
    advance(); // 跳过 [Namespace]
    return true;
}

bool CHTLParser::parseStyle(std::unique_ptr<CHTLNode>& style) {
    style = std::make_unique<StyleNode>();
    advance(); // 跳过 style
    
    if (match(TokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            // 解析样式内容
            advance();
        }
        
        if (!match(TokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLParser::parseScript(std::unique_ptr<CHTLNode>& script) {
    script = std::make_unique<ScriptNode>();
    advance(); // 跳过 script
    
    if (match(TokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            // 解析脚本内容
            advance();
        }
        
        if (!match(TokenType::RIGHT_BRACE)) {
            reportError("期望 }");
            return false;
        }
        advance(); // 跳过 }
    }
    
    return true;
}

bool CHTLParser::parseAttributes(std::unique_ptr<CHTLNode>& element) {
    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
        if (match(TokenType::IDENTIFIER)) {
            Token attrName = currentToken();
            advance();
            
            if (match({TokenType::COLON, TokenType::EQUAL})) {
                advance(); // 跳过 : 或 =
                
                if (match({TokenType::STRING, TokenType::LITERAL, TokenType::NUMBER})) {
                    Token attrValue = currentToken();
                    element->setAttribute(attrName.value, attrValue.value);
                    advance();
                } else {
                    reportError("期望属性值");
                    return false;
                }
            } else {
                reportError("期望 : 或 =");
                return false;
            }
        } else {
            advance(); // 跳过其他token
        }
    }
    
    return true;
}

bool CHTLParser::parseExpression(std::unique_ptr<CHTLNode>& expression) {
    // 简化实现
    expression = std::make_unique<ExpressionNode>("");
    return true;
}

Token CHTLParser::currentToken() const {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens_[currentToken_];
}

Token CHTLParser::peekToken(size_t offset) const {
    if (currentToken_ + offset >= tokens_.size()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens_[currentToken_ + offset];
}

bool CHTLParser::match(TokenType type) {
    return currentToken().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (currentToken().type == type) {
            return true;
        }
    }
    return false;
}

void CHTLParser::advance() {
    if (!isAtEnd()) {
        currentToken_++;
    }
}

bool CHTLParser::isAtEnd() const {
    return currentToken_ >= tokens_.size();
}

void CHTLParser::reportError(const std::string& message) {
    Token token = currentToken();
    reportError(message, token);
}

void CHTLParser::reportError(const std::string& message, const Token& token) {
    std::stringstream ss;
    ss << "解析错误 [" << token.line << ":" << token.column << "]: " << message;
    errors_.push_back(ss.str());
    std::cerr << ss.str() << std::endl;
}
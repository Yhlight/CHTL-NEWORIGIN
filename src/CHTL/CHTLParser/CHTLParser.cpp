#include "CHTLParser.h"
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser(const String& source, const ParserConfig& config)
    : config_(config) {
    Lexer lexer(source);
    tokens_ = lexer.tokenize();
}

CHTLParser::CHTLParser(Vector<Token> tokens, const ParserConfig& config)
    : tokens_(std::move(tokens)), config_(config) {}

SharedPtr<ProgramNode> CHTLParser::parse() {
    auto program = std::make_shared<ProgramNode>();
    
    // 初始化状态
    pushState(std::make_shared<InitialState>(this));
    
    try {
        while (!isAtEnd()) {
            auto node = parseTopLevel();
            if (node) {
                program->addChild(node);
            }
        }
    } catch (const CompileError& e) {
        error(e.what());
    }
    
    return program;
}

SharedPtr<BaseNode> CHTLParser::parseTopLevel() {
    Token token = getCurrentToken();
    
    // 跳过注释（如果配置允许）
    if (token.isComment()) {
        if (config_.enableComments) {
            return parseComment();
        } else {
            advance();
            return nullptr;
        }
    }
    
    // 根据Token类型选择解析方法
    if (token.is(TokenType::HtmlKeyword)) {
        return parseElement();
    } else if (token.is(TokenType::TextKeyword)) {
        return parseText();
    } else if (token.is(TokenType::Template)) {
        return parseTemplate();
    } else if (token.is(TokenType::Custom)) {
        return parseCustom();
    } else if (token.is(TokenType::Origin)) {
        return parseOrigin();
    } else if (token.is(TokenType::Import)) {
        return parseImport();
    } else {
        error("Unexpected token: " + token.toString());
        synchronize();
        return nullptr;
    }
}

SharedPtr<BaseNode> CHTLParser::parseStatement() {
    return parseTopLevel();
}

SharedPtr<ElementNode> CHTLParser::parseElement() {
    Token tagToken = advance();  // 消耗标签名
    
    auto element = std::make_shared<ElementNode>(tagToken.getValue(), 
                                                  SourceRange(tagToken.getPosition(), tagToken.getPosition()));
    
    expect(TokenType::LeftBrace, "Expected '{' after element name");
    
    // 解析元素体
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        Token token = getCurrentToken();
        
        // 注释
        if (token.isComment()) {
            if (config_.enableComments) {
                element->addChild(parseComment());
            } else {
                advance();
            }
            continue;
        }
        
        // 属性（identifier : value）
        if (token.is(TokenType::Identifier) && peek().is(TokenType::Colon)) {
            String attrName = advance().getValue();
            advance();  // 消耗 :
            String attrValue = parseAttributeValue();
            element->setAttribute(attrName, attrValue);
            
            if (check(TokenType::Semicolon)) {
                advance();
            }
            continue;
        }
        
        // text关键字
        if (token.is(TokenType::TextKeyword)) {
            if (peek().is(TokenType::Colon)) {
                // text: "value" 形式
                advance();  // 消耗text
                advance();  // 消耗:
                String textValue = parseAttributeValue();
                element->addChild(std::make_shared<TextNode>(textValue));
                if (check(TokenType::Semicolon)) {
                    advance();
                }
            } else {
                // text { } 形式
                element->addChild(parseText());
            }
            continue;
        }
        
        // style块
        if (token.is(TokenType::StyleKeyword)) {
            element->addChild(parseStyle());
            continue;
        }
        
        // script块
        if (token.is(TokenType::ScriptKeyword)) {
            element->addChild(parseScript());
            continue;
        }
        
        // 嵌套元素
        if (token.is(TokenType::HtmlKeyword)) {
            element->addChild(parseElement());
            continue;
        }
        
        // 模板使用
        if (token.is(TokenType::AtElement) || token.is(TokenType::AtStyle)) {
            // TODO: 处理模板引用
            advance();
            continue;
        }
        
        // 未识别的Token
        error("Unexpected token in element body: " + token.toString());
        advance();
    }
    
    expect(TokenType::RightBrace, "Expected '}' after element body");
    
    return element;
}

SharedPtr<TextNode> CHTLParser::parseText() {
    advance();  // 消耗text关键字
    expect(TokenType::LeftBrace, "Expected '{' after 'text'");
    
    String content = parseTextContent();
    
    expect(TokenType::RightBrace, "Expected '}' after text content");
    
    return std::make_shared<TextNode>(content);
}

SharedPtr<CommentNode> CHTLParser::parseComment() {
    Token token = advance();
    
    CommentNode::CommentType type;
    if (token.is(TokenType::LineComment)) {
        type = CommentNode::CommentType::Line;
    } else if (token.is(TokenType::BlockComment)) {
        type = CommentNode::CommentType::Block;
    } else {
        type = CommentNode::CommentType::Generator;
    }
    
    return std::make_shared<CommentNode>(token.getValue(), type, 
                                         SourceRange(token.getPosition(), token.getPosition()));
}

SharedPtr<StyleNode> CHTLParser::parseStyle() {
    advance();  // 消耗style关键字
    expect(TokenType::LeftBrace, "Expected '{' after 'style'");
    
    auto styleNode = std::make_shared<StyleNode>();
    
    // 解析style块内容
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        Token token = getCurrentToken();
        
        // 检查是否是CSS选择器（.class, #id, &:hover等）
        if (token.is(TokenType::Dot) || token.is(TokenType::Hash) || token.is(TokenType::Ampersand)) {
            String selector;
            
            if (token.is(TokenType::Dot)) {
                advance();
                selector = "." + expectToken(TokenType::Identifier, "Expected class name").getValue();
            } else if (token.is(TokenType::Hash)) {
                advance();
                selector = "#" + expectToken(TokenType::Identifier, "Expected id name").getValue();
            } else if (token.is(TokenType::Ampersand)) {
                advance();
                selector = "&";
                // 检查是否有伪类或伪元素
                if (check(TokenType::Colon)) {
                    advance();
                    if (check(TokenType::Colon)) {
                        // 伪元素 ::
                        advance();
                        selector += "::";
                    } else {
                        // 伪类 :
                        selector += ":";
                    }
                    if (check(TokenType::Identifier)) {
                        selector += advance().getValue();
                    }
                }
            }
            
            // 检查是否还有伪类/伪元素（如 .box:hover）
            if (check(TokenType::Colon)) {
                advance();
                if (check(TokenType::Colon)) {
                    // 伪元素 ::
                    advance();
                    selector += "::";
                } else {
                    selector += ":";
                }
                if (check(TokenType::Identifier)) {
                    selector += advance().getValue();
                }
            }
            
            expect(TokenType::LeftBrace, "Expected '{' after selector");
            String rules = parseBlockContent(TokenType::RightBrace);
            styleNode->addCssRule(selector, rules);
            expect(TokenType::RightBrace, "Expected '}' after CSS rules");
        }
        // 内联样式
        else if (token.is(TokenType::Identifier) && peek().is(TokenType::Colon)) {
            String property = advance().getValue();
            advance();  // 消耗:
            String value = parseAttributeValue();
            styleNode->addInlineStyle(property, value);
            
            if (check(TokenType::Semicolon)) {
                advance();
            }
        } else {
            advance();
        }
    }
    
    expect(TokenType::RightBrace, "Expected '}' after style block");
    
    return styleNode;
}

SharedPtr<ScriptNode> CHTLParser::parseScript() {
    advance();  // 消耗script关键字
    expect(TokenType::LeftBrace, "Expected '{' after 'script'");
    
    String content = parseBlockContent(TokenType::RightBrace);
    
    expect(TokenType::RightBrace, "Expected '}' after script block");
    
    auto scriptNode = std::make_shared<ScriptNode>(content);
    scriptNode->setLocal(true);  // 局部script块
    
    return scriptNode;
}

SharedPtr<TemplateNode> CHTLParser::parseTemplate() {
    advance();  // 消耗[Template]
    
    Token typeToken = advance();
    TemplateNode::TemplateType type;
    
    if (typeToken.is(TokenType::AtStyle)) {
        type = TemplateNode::TemplateType::Style;
    } else if (typeToken.is(TokenType::AtElement)) {
        type = TemplateNode::TemplateType::Element;
    } else if (typeToken.is(TokenType::AtVar)) {
        type = TemplateNode::TemplateType::Var;
    } else {
        error("Expected @Style, @Element, or @Var after [Template]");
        return nullptr;
    }
    
    Token nameToken = expectToken(TokenType::Identifier, "Expected template name");
    String name = nameToken.getValue();
    
    auto templateNode = std::make_shared<TemplateNode>(type, name);
    
    expect(TokenType::LeftBrace, "Expected '{' after template name");
    
    // 解析模板体（对于样式模板，直接解析CSS属性）
    if (type == TemplateNode::TemplateType::Style) {
        // 样式模板直接包含CSS属性
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            if (check(TokenType::Identifier) && peek().is(TokenType::Colon)) {
                String property = advance().getValue();
                advance();  // 消耗:
                String value = parseAttributeValue();
                
                // 创建一个临时的样式节点来存储
                auto styleNode = std::make_shared<StyleNode>();
                styleNode->addInlineStyle(property, value);
                templateNode->addChild(styleNode);
                
                if (check(TokenType::Semicolon)) {
                    advance();
                }
            } else {
                advance();
            }
        }
    } else {
        // 元素模板和变量组模板
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            auto node = parseStatement();
            if (node) {
                templateNode->addChild(node);
            }
        }
    }
    
    expect(TokenType::RightBrace, "Expected '}' after template body");
    
    return templateNode;
}

SharedPtr<CustomNode> CHTLParser::parseCustom() {
    advance();  // 消耗[Custom]
    
    Token typeToken = advance();
    CustomNode::CustomType type;
    
    if (typeToken.is(TokenType::AtStyle)) {
        type = CustomNode::CustomType::Style;
    } else if (typeToken.is(TokenType::AtElement)) {
        type = CustomNode::CustomType::Element;
    } else if (typeToken.is(TokenType::AtVar)) {
        type = CustomNode::CustomType::Var;
    } else {
        error("Expected @Style, @Element, or @Var after [Custom]");
        return nullptr;
    }
    
    Token nameToken = expectToken(TokenType::Identifier, "Expected custom name");
    String name = nameToken.getValue();
    
    auto customNode = std::make_shared<CustomNode>(type, name);
    
    expect(TokenType::LeftBrace, "Expected '{' after custom name");
    
    // 解析自定义体
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        auto node = parseStatement();
        if (node) {
            customNode->addChild(node);
        }
    }
    
    expect(TokenType::RightBrace, "Expected '}' after custom body");
    
    return customNode;
}

SharedPtr<OriginNode> CHTLParser::parseOrigin() {
    advance();  // 消耗[Origin]
    
    Token typeToken = advance();
    OriginNode::OriginType type;
    String typeName;
    
    if (typeToken.is(TokenType::AtHtml)) {
        type = OriginNode::OriginType::Html;
    } else if (typeToken.is(TokenType::AtStyle)) {
        type = OriginNode::OriginType::Style;
    } else if (typeToken.is(TokenType::AtJavaScript)) {
        type = OriginNode::OriginType::JavaScript;
    } else if (typeToken.is(TokenType::Identifier) && typeToken.getValue()[0] == '@') {
        type = OriginNode::OriginType::Custom;
        typeName = typeToken.getValue();
    } else {
        error("Expected @Html, @Style, @JavaScript, or custom type after [Origin]");
        return nullptr;
    }
    
    expect(TokenType::LeftBrace, "Expected '{' after origin type");
    
    String content = parseBlockContent(TokenType::RightBrace);
    
    expect(TokenType::RightBrace, "Expected '}' after origin content");
    
    auto originNode = std::make_shared<OriginNode>(type, content);
    if (type == OriginNode::OriginType::Custom) {
        originNode->setTypeName(typeName);
    }
    
    return originNode;
}

SharedPtr<ImportNode> CHTLParser::parseImport() {
    advance();  // 消耗[Import]
    
    // 简化处理：跳过import的详细解析
    // TODO: 实现完整的import解析
    while (!check(TokenType::Semicolon) && !isAtEnd()) {
        advance();
    }
    
    if (check(TokenType::Semicolon)) {
        advance();
    }
    
    return std::make_shared<ImportNode>("");
}

String CHTLParser::parseAttributeValue() {
    Token token = getCurrentToken();
    
    if (token.is(TokenType::StringLiteral)) {
        return advance().getValue();
    } else if (token.is(TokenType::NumberLiteral)) {
        return advance().getValue();
    } else if (token.is(TokenType::Identifier) || token.is(TokenType::UnquotedLiteral)) {
        return advance().getValue();
    } else {
        error("Expected attribute value");
        return "";
    }
}

String CHTLParser::parseTextContent() {
    String content;
    
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        Token token = advance();
        
        if (token.is(TokenType::StringLiteral)) {
            content += token.getValue();
        } else if (!token.isComment()) {
            content += token.getValue();
        }
    }
    
    return content;
}

String CHTLParser::parseBlockContent(TokenType endToken) {
    String content;
    int braceDepth = 0;
    
    while (!isAtEnd()) {
        Token token = getCurrentToken();
        
        // 特殊处理增强选择器 - 保留{{ }}标记
        if (token.is(TokenType::EnhancedSelector)) {
            content += "{{" + token.getValue() + "}}";
            advance();
            continue;
        }
        
        if (token.is(TokenType::LeftBrace)) {
            braceDepth++;
            content += advance().getValue();
        } else if (token.is(TokenType::RightBrace)) {
            if (braceDepth == 0 && endToken == TokenType::RightBrace) {
                break;
            }
            braceDepth--;
            content += advance().getValue();
        } else if (token.is(endToken) && braceDepth == 0) {
            break;
        } else {
            content += advance().getValue();
            content += " ";
        }
    }
    
    return content;
}

Token CHTLParser::getCurrentToken() const {
    if (current_ < tokens_.size()) {
        return tokens_[current_];
    }
    return tokens_.back();
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return tokens_[current_ - 1];
}

Token CHTLParser::peek(int offset) const {
    size_t index = current_ + offset;
    if (index < tokens_.size()) {
        return tokens_[index];
    }
    return tokens_.back();
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return getCurrentToken().is(type);
}

Token CHTLParser::expectToken(TokenType type, const String& message) {
    if (!check(type)) {
        error(message + ", got " + getCurrentToken().toString());
        throw CompileError(message, getCurrentToken().getPosition());
    }
    return advance();
}

void CHTLParser::expect(TokenType type, const String& message) {
    expectToken(type, message);
}

bool CHTLParser::isAtEnd() const {
    return current_ >= tokens_.size() || getCurrentToken().is(TokenType::EndOfFile);
}

void CHTLParser::pushState(SharedPtr<CHTLState> state) {
    stateStack_.push_back(state);
}

void CHTLParser::popState() {
    if (!stateStack_.empty()) {
        stateStack_.pop_back();
    }
}

SharedPtr<CHTLState> CHTLParser::getCurrentState() const {
    if (!stateStack_.empty()) {
        return stateStack_.back();
    }
    return nullptr;
}

void CHTLParser::setParseStrategy(SharedPtr<ParseStrategy> strategy) {
    currentStrategy_ = strategy;
}

SharedPtr<ParseStrategy> CHTLParser::getParseStrategy() const {
    return currentStrategy_;
}

void CHTLParser::error(const String& message) {
    Token token = getCurrentToken();
    String fullMessage = "[" + token.getPosition().toString() + "] " + message;
    errors_.push_back(fullMessage);
    
    if (config_.strictMode) {
        throw CompileError(message, token.getPosition());
    }
}

void CHTLParser::warning(const String& message) {
    Token token = getCurrentToken();
    String fullMessage = "[" + token.getPosition().toString() + "] " + message;
    warnings_.push_back(fullMessage);
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        Token token = getCurrentToken();
        
        // 在这些Token处停止同步
        if (token.is(TokenType::HtmlKeyword) ||
            token.is(TokenType::Template) ||
            token.is(TokenType::Custom) ||
            token.is(TokenType::Origin) ||
            token.is(TokenType::Import)) {
            return;
        }
        
        advance();
    }
}

} // namespace CHTL

#include "CHTLParser.h"
#include "ExpressionParser.h"
#include "ConditionalParser.h"
#include "../CHTLNode/ConditionalNode.h"
#include "../../Util/StringUtil/StringUtil.h"
#include "../../SharedCore/SaltBridge.h"
#include "../CHTLTemplate/TemplateRegistry.h"
#include <sstream>
#include <regex>

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
    
    // 清理SaltBridge注册表（避免测试间干扰）
    Bridge::SaltBridge::getInstance().clearRegistry();
    
    // 初始化状态
    pushState(std::make_shared<InitialState>(this));
    
    try {
        // 第一遍：解析所有节点
        while (!isAtEnd()) {
            auto node = parseTopLevel();
            if (node) {
                program->addChild(node);
            }
        }
        
        // 第二遍：收集元素注册信息（为属性引用做准备）
        collectElementsForRegistration(program);
        
        // 第三遍：重新评估所有包含属性引用的表达式
        reevaluateExpressionsWithReferences(program);
        
    } catch (const CompileError& e) {
        error(e.what());
    }
    
    return program;
}

void CHTLParser::collectElementsForRegistration(const SharedPtr<BaseNode>& node) {
    if (!node) {
        return;
    }
    
    if (node->getType() == NodeType::Element) {
        auto element = std::dynamic_pointer_cast<ElementNode>(node);
        
        // 收集内联样式
        HashMap<String, String> inlineStyles;
        for (const auto& child : element->getChildren()) {
            if (child->getType() == NodeType::Style) {
                auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
                const auto& styles = styleNode->getInlineStyles();
                inlineStyles.insert(styles.begin(), styles.end());
            }
        }
        
        // 注册到SaltBridge
        String elemId = element->hasAttribute("id") ? element->getAttribute("id").value() : "";
        String elemClass = element->hasAttribute("class") ? element->getAttribute("class").value() : "";
        Bridge::SaltBridge::getInstance().registerElementWithProperties(
            element->getTagName(), elemId, elemClass, inlineStyles);
    }
    
    // 递归处理子节点
    for (const auto& child : node->getChildren()) {
        collectElementsForRegistration(child);
    }
}

void CHTLParser::reevaluateExpressionsWithReferences(const SharedPtr<BaseNode>& /* node */) {
    // 属性引用表达式重新评估
    // 注意：当前架构中，表达式在解析时立即评估
    // 对于包含属性引用的表达式（如 width: @baseWidth * 2），
    // 我们在 parseExpressionValue 中已经处理了变量替换
    // 
    // 如果未来需要延迟评估，可以：
    // 1. 在 AST 节点中保存原始表达式 token
    // 2. 在此函数中遍历节点树，重新评估所有包含引用的表达式
    // 3. 更新节点的属性值
    //
    // 当前实现已经足够满足大多数场景，延迟评估可作为未来优化
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
        
        // @Element模板引用
        if (token.is(TokenType::AtElement)) {
            auto templateRef = parseElementTemplateReference();
            if (templateRef) {
                element->addChild(templateRef);
            }
            continue;
        }
        
        // @Style模板引用（已在parseStyle中处理）
        if (token.is(TokenType::AtStyle)) {
            advance();
            continue;
        }
        
        // if块（条件渲染）
        if (token.getValue() == "if") {
            auto conditional = parseConditional();
            if (conditional) {
                // 设置父元素选择器用于CSS生成
                auto condNode = std::dynamic_pointer_cast<ConditionalNode>(conditional);
                if (condNode) {
                    // 优先使用id，其次class，最后标签名
                    String selector;
                    if (element->hasAttribute("id")) {
                        selector = "#" + element->getAttribute("id").value();
                    } else if (element->hasAttribute("class")) {
                        selector = "." + element->getAttribute("class").value();
                    } else {
                        selector = element->getTagName();
                    }
                    condNode->setParentSelector(selector);
                }
                element->addChild(conditional);
            }
            continue;
        }
        
        // 未识别的Token
        error("Unexpected token in element body: " + token.toString());
        advance();
    }
    
    expect(TokenType::RightBrace, "Expected '}' after element body");
    
    // 立即注册元素到SaltBridge，以便后续元素可以引用其属性
    HashMap<String, String> inlineStyles;
    for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::Style) {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
            const auto& styles = styleNode->getInlineStyles();
            inlineStyles.insert(styles.begin(), styles.end());
        }
    }
    
    String elemId = element->hasAttribute("id") ? element->getAttribute("id").value() : "";
    String elemClass = element->hasAttribute("class") ? element->getAttribute("class").value() : "";
    Bridge::SaltBridge::getInstance().registerElementWithProperties(
        element->getTagName(), elemId, elemClass, inlineStyles);
    
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
            
            // 解析CSS规则块，处理其中的表达式
            String rules;
            while (!check(TokenType::RightBrace) && !isAtEnd()) {
                Token propToken = getCurrentToken();
                
                // 跳过注释
                if (propToken.isComment()) {
                    advance();
                    continue;
                }
                
                if (propToken.is(TokenType::Identifier) && peek().is(TokenType::Colon)) {
                    String property = advance().getValue();
                    advance();  // 消耗:
                    String value = parseAttributeValue();  // 使用支持表达式的解析
                    
                    rules += property + " : " + value + " ; ";
                    
                    if (check(TokenType::Semicolon)) {
                        advance();
                    }
                } else {
                    advance();
                }
            }
            
            styleNode->addCssRule(selector, rules);
            expect(TokenType::RightBrace, "Expected '}' after CSS rules");
        }
        // 模板引用 @Style TemplateName;
        else if (token.is(TokenType::AtStyle)) {
            advance();  // 消耗@Style
            
            // 获取模板名称
            Token nameToken = getCurrentToken();
            if (!nameToken.is(TokenType::Identifier) && !nameToken.is(TokenType::HtmlKeyword)) {
                error("Expected template name after @Style");
                advance();
                continue;
            }
            String templateName = advance().getValue();
            
            // 从TemplateRegistry获取模板
            auto templateDef = TemplateRegistry::getInstance().findTemplate(
                templateName, TemplateNode::TemplateType::Style);
            
            if (templateDef.has_value()) {
                // 展开模板：将模板中的样式添加到当前styleNode
                for (const auto& child : templateDef->children) {
                    if (child->getType() == NodeType::Style) {
                        auto templateStyle = std::dynamic_pointer_cast<StyleNode>(child);
                        const auto& inlineStyles = templateStyle->getInlineStyles();
                        for (const auto& [prop, val] : inlineStyles) {
                            styleNode->addInlineStyle(prop, val);
                        }
                    }
                }
            }
            
            if (check(TokenType::Semicolon)) {
                advance();
            }
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
    
    // 模板名称可以是Identifier或HtmlKeyword
    Token nameToken = getCurrentToken();
    if (!nameToken.is(TokenType::Identifier) && !nameToken.is(TokenType::HtmlKeyword)) {
        error("Expected template name, got " + nameToken.toString());
        return nullptr;
    }
    String name = advance().getValue();
    
    auto templateNode = std::make_shared<TemplateNode>(type, name);
    
    expect(TokenType::LeftBrace, "Expected '{' after template name");
    
    // 解析模板体
    if (type == TemplateNode::TemplateType::Style || type == TemplateNode::TemplateType::Var) {
        // 样式模板和变量组模板直接包含CSS属性格式
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            if ((check(TokenType::Identifier) || check(TokenType::HtmlKeyword)) && peek().is(TokenType::Colon)) {
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
        // 元素模板
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            auto node = parseStatement();
            if (node) {
                templateNode->addChild(node);
            }
        }
    }
    
    expect(TokenType::RightBrace, "Expected '}' after template body");
    
    // 注册模板到TemplateRegistry
    if (type == TemplateNode::TemplateType::Var) {
        // 对于变量组，提取变量
        HashMap<String, String> variables;
        for (const auto& child : templateNode->getChildren()) {
            if (child->getType() == NodeType::Style) {
                auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
                const auto& inlineStyles = styleNode->getInlineStyles();
                variables.insert(inlineStyles.begin(), inlineStyles.end());
            }
        }
        TemplateRegistry::getInstance().registerVarTemplate(name, variables);
    } else {
        // 对于样式模板和元素模板
        TemplateRegistry::getInstance().registerTemplate(name, type, templateNode->getChildren());
    }
    
    return templateNode;
}

SharedPtr<BaseNode> CHTLParser::parseElementTemplateReference() {
    advance();  // 消耗@Element
    
    // 获取模板名称
    Token nameToken = getCurrentToken();
    if (!nameToken.is(TokenType::Identifier) && !nameToken.is(TokenType::HtmlKeyword)) {
        error("Expected template name after @Element");
        return nullptr;
    }
    String templateName = advance().getValue();
    
    // 消耗分号（如果存在）
    if (check(TokenType::Semicolon)) {
        advance();
    }
    
    // 从TemplateRegistry获取模板
    auto templateDef = TemplateRegistry::getInstance().findTemplate(
        templateName, TemplateNode::TemplateType::Element);
    
    if (!templateDef.has_value()) {
        error("Element template not found: " + templateName);
        return nullptr;
    }
    
    // 创建一个容器节点来包含模板展开的所有子节点
    // 我们使用ProgramNode作为容器，因为它可以包含多个子节点
    auto container = std::make_shared<ProgramNode>();
    
    // 展开模板：克隆模板中的所有子节点
    for (const auto& child : templateDef->children) {
        if (child) {
            container->addChild(child->clone());
        }
    }
    
    return container;
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
    
    // Import 语句的完整解析在 ModuleResolver 中完成
    // 这里只需要提取原始 import 内容，由模块系统处理
    
    String importContent;
    
    while (!check(TokenType::Semicolon) && !isAtEnd()) {
        Token token = getCurrentToken();
        importContent += token.getValue() + " ";
        advance();
    }
    
    if (check(TokenType::Semicolon)) {
        advance();
    }
    
    // 创建 ImportNode 包含完整内容
    // ModuleResolver::parseImport 会处理所有 14 种导入类型
    auto importNode = std::make_shared<ImportNode>(importContent);
    
    return importNode;
}

String CHTLParser::parseAttributeValue() {
    Token token = getCurrentToken();
    
    // 对于字符串字面量，直接返回
    if (token.is(TokenType::StringLiteral)) {
        return advance().getValue();
    }
    
    // 对于可能包含表达式的值，使用parseExpressionValue
    return parseExpressionValue();
}

String CHTLParser::parseExpressionValue() {
    // 收集所有token直到遇到分隔符（; , } 等）
    Vector<Token> exprTokens;
    int parenDepth = 0;
    
    while (!isAtEnd()) {
        Token token = getCurrentToken();
        
        // 遇到分隔符且不在括号内，停止
        if (parenDepth == 0) {
            if (token.is(TokenType::Semicolon) || 
                token.is(TokenType::Comma) ||
                token.is(TokenType::RightBrace)) {
                break;
            }
        }
        
        // 跟踪括号深度
        if (token.is(TokenType::LeftParen)) {
            parenDepth++;
        } else if (token.is(TokenType::RightParen)) {
            parenDepth--;
            if (parenDepth < 0) break;
        }
        
        exprTokens.push_back(token);
        advance();
    }
    
    // 如果只有一个token，直接返回
    if (exprTokens.size() == 1) {
        return exprTokens[0].getValue();
    }
    
    // 检查并处理变量组引用：VarName(propertyName)
    // propertyName可以是Identifier或HtmlKeyword
    if (exprTokens.size() >= 4 && 
        exprTokens[0].is(TokenType::Identifier) &&
        exprTokens[1].is(TokenType::LeftParen) &&
        (exprTokens[2].is(TokenType::Identifier) || exprTokens[2].is(TokenType::HtmlKeyword)) &&
        exprTokens[3].is(TokenType::RightParen)) {
        
        String varGroupName = exprTokens[0].getValue();
        String propertyName = exprTokens[2].getValue();
        
        // 从TemplateRegistry查找变量组
        auto varTemplate = TemplateRegistry::getInstance().findVarTemplate(varGroupName);
        if (varTemplate.has_value()) {
            auto it = varTemplate->find(propertyName);
            if (it != varTemplate->end()) {
                String varValue = it->second;
                
                // 如果后面还有其他token（比如运算符），需要继续处理
                if (exprTokens.size() > 4) {
                    // 将变量值替换到表达式中
                    Vector<Token> newTokens;
                    // 解析变量值以获取正确的token类型
                    // 如果值包含单位（如10px），应该作为NumberLiteral
                    if (std::regex_match(varValue, std::regex(R"(\d+(\.\d+)?(px|em|rem|%|vh|vw|vmin|vmax)?)"))) {
                        newTokens.push_back(Token(TokenType::NumberLiteral, varValue, exprTokens[0].getPosition()));
                    } else {
                        newTokens.push_back(Token(TokenType::UnquotedLiteral, varValue, exprTokens[0].getPosition()));
                    }
                    // 添加剩余的tokens
                    for (size_t i = 4; i < exprTokens.size(); ++i) {
                        newTokens.push_back(exprTokens[i]);
                    }
                    exprTokens = newTokens;
                } else {
                    // 直接返回变量值
                    return varValue;
                }
            }
        }
    }
    
    // 检查是否包含运算符或属性引用（selector.property）
    bool hasOperator = false;
    for (size_t i = 0; i < exprTokens.size(); ++i) {
        const auto& t = exprTokens[i];
        if (t.is(TokenType::Plus) || t.is(TokenType::Minus) ||
            t.is(TokenType::Star) || t.is(TokenType::Slash) ||
            t.is(TokenType::Percent) || t.is(TokenType::Power) ||
            t.is(TokenType::Question) || t.is(TokenType::Greater) ||
            t.is(TokenType::Less) || t.is(TokenType::EqualEqual)) {
            hasOperator = true;
            break;
        }
        // 检查属性引用：identifier . identifier
        if (t.is(TokenType::Dot) && i > 0 && i + 1 < exprTokens.size()) {
            if (exprTokens[i-1].is(TokenType::Identifier) && 
                exprTokens[i+1].is(TokenType::Identifier)) {
                hasOperator = true;  // 属性引用需要表达式解析
                break;
            }
        }
    }
    
    // 如果包含运算符，使用表达式解析器
    if (hasOperator && !exprTokens.empty()) {
        try {
            // 添加EOF token
            exprTokens.push_back(Token(TokenType::EndOfFile, "", Position()));
            
            ExpressionParser exprParser(exprTokens);
            auto expr = exprParser.parse();
            if (expr) {
                return expr->evaluate();
            }
        } catch (...) {
            // 表达式解析失败，返回原始值
        }
    }
    
    // 否则，拼接所有token的值
    String result;
    for (const auto& t : exprTokens) {
        result += t.getValue();
        if (t.isNot(TokenType::Dot) && t.isNot(TokenType::LeftParen) && t.isNot(TokenType::RightParen)) {
            result += " ";
        }
    }
    return Util::StringUtil::trim(result);
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
        
        // 跳过生成器注释（在script/style块中）
        if (token.is(TokenType::GeneratorComment)) {
            advance();
            continue;
        }
        
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

SharedPtr<BaseNode> CHTLParser::parseConditional() {
    // 使用ConditionalParser解析条件渲染块
    // 这里需要ConditionalParser，但它需要position参数
    // 为了简单起见，我们直接在这里解析基础的if块结构
    
    // 期望'if'关键字
    if (getCurrentToken().getValue() != "if") {
        error("Expected 'if' keyword for conditional block");
        return nullptr;
    }
    advance();  // 消耗'if'
    
    // 期望'{'
    if (!check(TokenType::LeftBrace)) {
        error("Expected '{' after 'if'");
        return nullptr;
    }
    advance();  // 消耗'{'
    
    // 创建ConditionalNode（简化实现）
    auto conditionalNode = std::make_shared<ConditionalNode>();
    
    // 解析条件块内容
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        Token token = getCurrentToken();
        
        // condition: 关键字
        if (token.getValue() == "condition" && peek().is(TokenType::Colon)) {
            advance();  // 消耗'condition'
            advance();  // 消耗':'
            
            // 解析条件表达式（直到逗号或右括号）
            std::stringstream condition;
            bool hasDynamicSelector = false;
            
            while (!check(TokenType::Comma) && !check(TokenType::RightBrace) && !isAtEnd()) {
                Token condToken = getCurrentToken();
                
                // 检测EnhancedSelector类型（{{}}）
                if (condToken.is(TokenType::EnhancedSelector)) {
                    condition << "{{" << condToken.getValue() << "}}";
                    hasDynamicSelector = true;
                } else {
                    condition << condToken.getValue();
                }
                
                advance();
            }
            
            String conditionStr = Util::StringUtil::trim(condition.str());
            conditionalNode->setCondition(conditionStr);
            
            // 检测是否是动态条件（包含EnhancedSelector token）
            if (hasDynamicSelector || conditionStr.find("{{") != String::npos) {
                conditionalNode->setDynamic(true);
            }
            
            // 消耗逗号（如果有）
            if (check(TokenType::Comma)) {
                advance();
            }
            continue;
        }
        
        // CSS属性：property: value,
        if (token.is(TokenType::Identifier) && peek().is(TokenType::Colon)) {
            String propName = advance().getValue();
            advance();  // 消耗':'
            
            String propValue = parseAttributeValue();
            conditionalNode->addStyle(propName, propValue);
            
            // 消耗逗号或分号（如果有）
            if (check(TokenType::Comma) || check(TokenType::Semicolon)) {
                advance();
            }
            continue;
        }
        
        // 其他情况，跳过
        advance();
    }
    
    // 期望'}'
    if (!check(TokenType::RightBrace)) {
        error("Expected '}' to close 'if' block");
        return nullptr;
    }
    advance();  // 消耗'}'
    
    // 检查是否有else if或else
    while (getCurrentToken().getValue() == "else" && !isAtEnd()) {
        Token elseToken = getCurrentToken();
        advance();  // 消耗'else'
        
        // 检查是否是'else if'
        if (getCurrentToken().getValue() == "if") {
            // 递归解析else if
            auto elseIfNode = parseConditional();
            if (elseIfNode) {
                auto elseIfCond = std::dynamic_pointer_cast<ConditionalNode>(elseIfNode);
                if (elseIfCond) {
                    // else if块继承if块的动态状态（如果if是动态的）
                    if (conditionalNode->isDynamic()) {
                        elseIfCond->setDynamic(true);
                    }
                    conditionalNode->addElseIfBlock(elseIfCond);
                }
            }
        } else if (check(TokenType::LeftBrace)) {
            // 纯else块
            advance();  // 消耗'{'
            
            auto elseNode = std::make_shared<ConditionalNode>();
            elseNode->setBlockType(ConditionalNode::BlockType::Else);
            
            // else块继承if块的动态状态
            elseNode->setDynamic(conditionalNode->isDynamic());
            
            // 解析else块内容
            while (!check(TokenType::RightBrace) && !isAtEnd()) {
                Token token = getCurrentToken();
                
                // CSS属性
                if (token.is(TokenType::Identifier) && peek().is(TokenType::Colon)) {
                    String propName = advance().getValue();
                    advance();  // 消耗':'
                    
                    String propValue = parseAttributeValue();
                    elseNode->addStyle(propName, propValue);
                    
                    if (check(TokenType::Comma) || check(TokenType::Semicolon)) {
                        advance();
                    }
                    continue;
                }
                
                advance();
            }
            
            expect(TokenType::RightBrace, "Expected '}' to close 'else' block");
            conditionalNode->setElseBlock(elseNode);
            
            break;  // else是最后一个块
        } else {
            error("Expected 'if' or '{' after 'else'");
            break;
        }
    }
    
    return conditionalNode;
}

} // namespace CHTL

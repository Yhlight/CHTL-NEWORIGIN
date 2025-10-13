#pragma once

#include "../Common.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "Node.hpp"

namespace CHTL {

// 语法分析器
class Parser {
public:
    explicit Parser(Vec<Token> tokens)
        : tokens_(std::move(tokens)), current_(0) {}
    
    // 解析整个文档
    Ptr<Node> parse() {
        auto root = std::make_shared<ElementNode>("root");
        
        while (!isAtEnd()) {
            if (auto node = parseTopLevel()) {
                root->addChild(node);
            }
        }
        
        return root;
    }
    
private:
    Vec<Token> tokens_;
    size_t current_;
    
    bool isAtEnd() const {
        return current_ >= tokens_.size() || 
               peek().is(TokenType::EndOfFile);
    }
    
    const Token& peek() const {
        return tokens_[current_];
    }
    
    const Token& previous() const {
        return tokens_[current_ - 1];
    }
    
    const Token& advance() {
        if (!isAtEnd()) current_++;
        return previous();
    }
    
    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().is(type);
    }
    
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
    
    Token consume(TokenType type, const String& message) {
        if (check(type)) return advance();
        
        throw CompileError(message, peek().line(), peek().column());
    }
    
    // 解析顶层节点
    Ptr<Node> parseTopLevel() {
        // 跳过注释
        if (match(TokenType::Comment)) {
            return std::make_shared<CommentNode>(previous().value(), false);
        }
        
        if (match(TokenType::GeneratorComment)) {
            return std::make_shared<CommentNode>(previous().value(), true);
        }
        
        // [Template]
        if (match(TokenType::Template)) {
            return parseTemplate();
        }
        
        // [Custom]
        if (match(TokenType::Custom)) {
            return parseCustom();
        }
        
        // [Origin]
        if (match(TokenType::Origin)) {
            return parseOrigin();
        }
        
        // [Import]
        if (match(TokenType::Import)) {
            return parseImport();
        }
        
        // [Namespace]
        if (match(TokenType::Namespace)) {
            return parseNamespace();
        }
        
        // [Configuration]
        if (match(TokenType::Configuration)) {
            return parseConfiguration();
        }
        
        // HTML元素
        if (check(TokenType::Identifier)) {
            return parseElement();
        }
        
        // text块
        if (match(TokenType::Text)) {
            return parseText();
        }
        
        // style块（全局）
        if (match(TokenType::Style)) {
            return parseGlobalStyle();
        }
        
        // script块（全局）
        if (match(TokenType::Script)) {
            return parseGlobalScript();
        }
        
        // 跳过未知token
        advance();
        return nullptr;
    }
    
    // 解析元素
    Ptr<Node> parseElement() {
        Token nameToken = consume(TokenType::Identifier, "Expected element name");
        auto element = std::make_shared<ElementNode>(nameToken.value());
        
        // 元素必须有{}块
        consume(TokenType::LeftBrace, "Expected '{' after element name");
        
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            // 属性
            if (check(TokenType::Identifier) && checkNext(TokenType::Colon, TokenType::Equal)) {
                parseAttribute(element);
            }
            // text块
            else if (match(TokenType::Text)) {
                element->addChild(parseText());
            }
            // style块（局部）
            else if (match(TokenType::Style)) {
                element->addChild(parseLocalStyle());
            }
            // script块（局部）
            else if (match(TokenType::Script)) {
                element->addChild(parseLocalScript());
            }
            // if块
            else if (match(TokenType::If)) {
                element->addChild(parseIf());
            }
            // 子元素
            else if (check(TokenType::Identifier)) {
                element->addChild(parseElement());
            }
            // 注释
            else if (match(TokenType::Comment)) {
                element->addChild(std::make_shared<CommentNode>(previous().value(), false));
            }
            else if (match(TokenType::GeneratorComment)) {
                element->addChild(std::make_shared<CommentNode>(previous().value(), true));
            }
            else {
                advance(); // 跳过未知token
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after element body");
        
        return element;
    }
    
    // 解析属性
    void parseAttribute(Ptr<ElementNode> element) {
        Token nameToken = consume(TokenType::Identifier, "Expected attribute name");
        
        // : 或 = 都可以
        if (!match(TokenType::Colon) && !match(TokenType::Equal)) {
            throw CompileError("Expected ':' or '=' after attribute name",
                             peek().line(), peek().column());
        }
        
        String value;
        if (check(TokenType::String)) {
            value = advance().value();
        } else if (check(TokenType::Identifier) || check(TokenType::Number)) {
            value = advance().value(); // 无修饰字面量
        } else {
            throw CompileError("Expected attribute value",
                             peek().line(), peek().column());
        }
        
        match(TokenType::Semicolon); // 可选的分号
        
        element->addAttribute(nameToken.value(), value);
    }
    
    // 解析text块
    Ptr<Node> parseText() {
        consume(TokenType::LeftBrace, "Expected '{' after 'text'");
        
        String content;
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            if (check(TokenType::String)) {
                content += advance().value();
            } else if (check(TokenType::Identifier) || check(TokenType::Number)) {
                content += advance().value(); // 无修饰字面量
            } else {
                advance(); // 跳过其他token
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after text block");
        
        return std::make_shared<TextNode>(content);
    }
    
    // 解析局部样式块
    Ptr<Node> parseLocalStyle() {
        auto style = std::make_shared<StyleNode>();
        
        consume(TokenType::LeftBrace, "Expected '{' after 'style'");
        
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            // CSS属性
            if (check(TokenType::Identifier)) {
                Token propName = advance();
                consume(TokenType::Colon, "Expected ':' after property name");
                
                String value;
                // 收集值直到遇到分号
                while (!check(TokenType::Semicolon) && !check(TokenType::RightBrace) && !isAtEnd()) {
                    value += advance().value() + " ";
                }
                
                match(TokenType::Semicolon); // 可选的分号
                
                style->addProperty(propName.value(), Util::StringUtil::trim(value));
            }
            // 类选择器 .box
            else if (match(TokenType::Dot)) {
                // TODO: 处理类选择器
                advance(); // 跳过类名
            }
            // # id选择器
            else if (check(TokenType::Identifier) && peek().value()[0] == '#') {
                // TODO: 处理id选择器
                advance();
            }
            else {
                advance(); // 跳过未知token
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after style block");
        
        return style;
    }
    
    // 解析局部脚本块
    Ptr<Node> parseLocalScript() {
        consume(TokenType::LeftBrace, "Expected '{' after 'script'");
        
        String content;
        int braceCount = 1;
        
        // 收集整个脚本内容，处理嵌套的{}
        while (braceCount > 0 && !isAtEnd()) {
            if (check(TokenType::LeftBrace)) {
                braceCount++;
                content += advance().value();
            } else if (check(TokenType::RightBrace)) {
                braceCount--;
                if (braceCount > 0) {
                    content += advance().value();
                }
            } else {
                content += advance().value() + " ";
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after script block");
        
        return std::make_shared<ScriptNode>(Util::StringUtil::trim(content));
    }
    
    // 解析全局样式
    Ptr<Node> parseGlobalStyle() {
        return parseLocalStyle(); // 暂时使用相同的逻辑
    }
    
    // 解析全局脚本
    Ptr<Node> parseGlobalScript() {
        return parseLocalScript(); // 暂时使用相同的逻辑
    }
    
    // 解析模板
    Ptr<Node> parseTemplate() {
        // @Style, @Element, @Var
        if (!check(TokenType::AtStyle) && !check(TokenType::AtElement) && 
            !check(TokenType::AtVar)) {
            throw CompileError("Expected template type after [Template]",
                             peek().line(), peek().column());
        }
        
        advance(); // 类型token
        Token nameToken = consume(TokenType::Identifier, "Expected template name");
        
        auto node = std::make_shared<TemplateNode>(NodeType::Template, nameToken.value());
        
        consume(TokenType::LeftBrace, "Expected '{' after template name");
        
        // 解析模板内容
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            if (auto child = parseTopLevel()) {
                node->addChild(child);
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after template body");
        
        return node;
    }
    
    // 解析自定义
    Ptr<Node> parseCustom() {
        // 类似模板
        advance(); // 类型token
        Token nameToken = consume(TokenType::Identifier, "Expected custom name");
        
        auto node = std::make_shared<CustomNode>(NodeType::Custom, nameToken.value());
        
        consume(TokenType::LeftBrace, "Expected '{' after custom name");
        
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            if (auto child = parseTopLevel()) {
                node->addChild(child);
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after custom body");
        
        return node;
    }
    
    // 解析原始嵌入
    Ptr<Node> parseOrigin() {
        // @Html, @Style, @JavaScript等
        Token typeToken = advance();
        
        Opt<String> name;
        if (check(TokenType::Identifier)) {
            name = advance().value();
        }
        
        consume(TokenType::LeftBrace, "Expected '{' after origin type");
        
        String content;
        int braceCount = 1;
        
        while (braceCount > 0 && !isAtEnd()) {
            if (check(TokenType::LeftBrace)) {
                braceCount++;
                content += advance().value();
            } else if (check(TokenType::RightBrace)) {
                braceCount--;
                if (braceCount > 0) {
                    content += advance().value();
                }
            } else {
                content += advance().value() + " ";
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after origin block");
        
        return std::make_shared<OriginNode>(typeToken.value(), 
                                           Util::StringUtil::trim(content), 
                                           name);
    }
    
    // 解析导入
    Ptr<Node> parseImport() {
        String importType;
        
        // 收集导入类型
        while (!check(TokenType::From) && !isAtEnd()) {
            importType += advance().value() + " ";
        }
        
        consume(TokenType::From, "Expected 'from' in import statement");
        
        Token pathToken = advance();
        
        Opt<String> alias;
        if (match(TokenType::As)) {
            alias = consume(TokenType::Identifier, "Expected alias after 'as'").value();
        }
        
        match(TokenType::Semicolon); // 可选的分号
        
        return std::make_shared<ImportNode>(Util::StringUtil::trim(importType),
                                           pathToken.value(),
                                           alias);
    }
    
    // 解析命名空间
    Ptr<Node> parseNamespace() {
        Token nameToken = consume(TokenType::Identifier, "Expected namespace name");
        auto node = std::make_shared<NamespaceNode>(nameToken.value());
        
        if (match(TokenType::LeftBrace)) {
            while (!check(TokenType::RightBrace) && !isAtEnd()) {
                if (auto child = parseTopLevel()) {
                    node->addChild(child);
                }
            }
            consume(TokenType::RightBrace, "Expected '}' after namespace body");
        }
        
        return node;
    }
    
    // 解析配置
    Ptr<Node> parseConfiguration() {
        Opt<String> name;
        
        // 可选的配置名称
        if (check(TokenType::AtConfig)) {
            advance();
            name = consume(TokenType::Identifier, "Expected configuration name").value();
        }
        
        auto node = std::make_shared<ConfigurationNode>(name);
        
        consume(TokenType::LeftBrace, "Expected '{' after configuration");
        
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            if (check(TokenType::Identifier)) {
                Token key = advance();
                consume(TokenType::Equal, "Expected '=' after configuration key");
                Token value = advance();
                match(TokenType::Semicolon);
                
                node->addSetting(key.value(), value.value());
            } else {
                advance(); // 跳过未知token
            }
        }
        
        consume(TokenType::RightBrace, "Expected '}' after configuration body");
        
        return node;
    }
    
    // 解析if块
    Ptr<Node> parseIf() {
        // TODO: 实现完整的if条件解析
        consume(TokenType::LeftBrace, "Expected '{' after 'if'");
        
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            advance();
        }
        
        consume(TokenType::RightBrace, "Expected '}' after if block");
        
        return std::make_shared<ElementNode>("if");
    }
    
    // 辅助函数：检查下一个token
    bool checkNext(TokenType type1) const {
        if (current_ + 1 >= tokens_.size()) return false;
        return tokens_[current_ + 1].is(type1);
    }
    
    bool checkNext(TokenType type1, TokenType type2) const {
        if (current_ + 1 >= tokens_.size()) return false;
        return tokens_[current_ + 1].is(type1) || tokens_[current_ + 1].is(type2);
    }
};

} // namespace CHTL

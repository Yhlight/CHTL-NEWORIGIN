#include "CHTLState.h"
#include "../CHTLParser/CHTLParser.h"

namespace CHTL {

// CHTLState基类实现
CHTLState::CHTLState(StateType type, CHTLParser* parser)
    : type_(type), parser_(parser) {}

SharedPtr<CHTLState> CHTLState::transition(const Token& /* token */) {
    // 默认实现：不转换状态
    return nullptr;
}

void CHTLState::handleError(const String& message) {
    if (parser_) {
        parser_->error(message);
    }
}

Token CHTLState::getCurrentToken() const {
    return parser_ ? parser_->getCurrentToken() : Token();
}

Token CHTLState::advance() {
    return parser_ ? parser_->advance() : Token();
}

Token CHTLState::peek() const {
    return parser_ ? parser_->peek() : Token();
}

bool CHTLState::match(TokenType type) {
    return parser_ ? parser_->match(type) : false;
}

bool CHTLState::check(TokenType type) const {
    return parser_ ? parser_->check(type) : false;
}

void CHTLState::expect(TokenType type, const String& message) {
    if (parser_) {
        parser_->expect(type, message);
    }
}

// InitialState实现
InitialState::InitialState(CHTLParser* parser)
    : CHTLState(StateType::Initial, parser) {}

SharedPtr<BaseNode> InitialState::parse() {
    // 初始状态不直接解析，由Parser处理
    return nullptr;
}

SharedPtr<CHTLState> InitialState::transition(const Token& token) {
    // 根据Token类型转换到相应的状态
    if (token.is(TokenType::HtmlKeyword)) {
        return std::make_shared<ElementState>(parser_);
    } else if (token.is(TokenType::TextKeyword)) {
        return std::make_shared<TextState>(parser_);
    } else if (token.is(TokenType::StyleKeyword)) {
        return std::make_shared<StyleState>(parser_);
    } else if (token.is(TokenType::ScriptKeyword)) {
        return std::make_shared<ScriptState>(parser_);
    } else if (token.is(TokenType::Template)) {
        return std::make_shared<TemplateState>(parser_);
    } else if (token.is(TokenType::Custom)) {
        return std::make_shared<CustomState>(parser_);
    } else if (token.isComment()) {
        return std::make_shared<CommentState>(parser_);
    } else if (token.getValue() == "if") {
        // 条件渲染状态 (if关键字)
        return std::make_shared<ConditionalState>(parser_);
    }
    
    return nullptr;
}

bool InitialState::canHandle(const Token& /* token */) const {
    // 初始状态可以处理任何Token
    return true;
}

// ElementState实现
ElementState::ElementState(CHTLParser* parser)
    : CHTLState(StateType::Element, parser) {}

SharedPtr<BaseNode> ElementState::parse() {
    return parseElement();
}

bool ElementState::canHandle(const Token& token) const {
    return token.is(TokenType::HtmlKeyword);
}

SharedPtr<BaseNode> ElementState::parseElement() {
    // 实际解析由Parser完成
    return nullptr;
}

Vector<SharedPtr<BaseNode>> ElementState::parseElementBody() {
    // 实际解析由Parser完成
    return {};
}

// AttributeState实现
AttributeState::AttributeState(CHTLParser* parser)
    : CHTLState(StateType::Attribute, parser) {}

SharedPtr<BaseNode> AttributeState::parse() {
    return nullptr;
}

bool AttributeState::canHandle(const Token& token) const {
    return token.is(TokenType::Identifier);
}

String AttributeState::parseAttributeValue() {
    return "";
}

// TextState实现
TextState::TextState(CHTLParser* parser)
    : CHTLState(StateType::Text, parser) {}

SharedPtr<BaseNode> TextState::parse() {
    return nullptr;
}

bool TextState::canHandle(const Token& token) const {
    return token.is(TokenType::TextKeyword);
}

String TextState::parseTextContent() {
    return "";
}

// StyleState实现
StyleState::StyleState(CHTLParser* parser)
    : CHTLState(StateType::Style, parser) {}

SharedPtr<BaseNode> StyleState::parse() {
    return nullptr;
}

bool StyleState::canHandle(const Token& token) const {
    return token.is(TokenType::StyleKeyword);
}

String StyleState::parseStyleContent() {
    return "";
}

// ScriptState实现
ScriptState::ScriptState(CHTLParser* parser)
    : CHTLState(StateType::Script, parser) {}

SharedPtr<BaseNode> ScriptState::parse() {
    return nullptr;
}

bool ScriptState::canHandle(const Token& token) const {
    return token.is(TokenType::ScriptKeyword);
}

String ScriptState::parseScriptContent() {
    return "";
}

// TemplateState实现
TemplateState::TemplateState(CHTLParser* parser)
    : CHTLState(StateType::Template, parser) {}

SharedPtr<BaseNode> TemplateState::parse() {
    return nullptr;
}

bool TemplateState::canHandle(const Token& token) const {
    return token.is(TokenType::Template);
}

// CustomState实现
CustomState::CustomState(CHTLParser* parser)
    : CHTLState(StateType::Custom, parser) {}

SharedPtr<BaseNode> CustomState::parse() {
    return nullptr;
}

bool CustomState::canHandle(const Token& token) const {
    return token.is(TokenType::Custom);
}

// CommentState实现
CommentState::CommentState(CHTLParser* parser)
    : CHTLState(StateType::Comment, parser) {}

SharedPtr<BaseNode> CommentState::parse() {
    return nullptr;
}

bool CommentState::canHandle(const Token& token) const {
    return token.isComment();
}

// ConditionalState实现
ConditionalState::ConditionalState(CHTLParser* parser)
    : CHTLState(StateType::Conditional, parser) {}

SharedPtr<BaseNode> ConditionalState::parse() {
    return parseConditionalBlock();
}

bool ConditionalState::canHandle(const Token& token) const {
    // 可以处理 'if', 'else if', 'else' 关键字
    return token.getValue() == "if" || token.getValue() == "else";
}

SharedPtr<BaseNode> ConditionalState::parseConditionalBlock() {
    // 实际解析由Parser和ConditionalParser完成
    // 这里只是状态机的占位符
    return nullptr;
}

} // namespace CHTL

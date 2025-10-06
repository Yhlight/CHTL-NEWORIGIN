#include "Token.h"

namespace CHTL {

bool Token::isKeyword() const {
    return type >= TokenType::HtmlKeyword && type <= TokenType::UseKeyword;
}

bool Token::isOperator() const {
    return type >= TokenType::Equal && type <= TokenType::Dollar;
}

bool Token::isLiteral() const {
    return type >= TokenType::Identifier && type <= TokenType::NumberLiteral;
}

bool Token::isComment() const {
    return type >= TokenType::LineComment && type <= TokenType::GeneratorComment;
}

String Token::toString() const {
    return typeToString() + "(\"" + value + "\") at " + position.toString();
}

String Token::typeToString() const {
    return tokenTypeToString(type);
}

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Unknown: return "Unknown";
        case TokenType::Identifier: return "Identifier";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::UnquotedLiteral: return "UnquotedLiteral";
        case TokenType::NumberLiteral: return "NumberLiteral";
        case TokenType::LineComment: return "LineComment";
        case TokenType::BlockComment: return "BlockComment";
        case TokenType::GeneratorComment: return "GeneratorComment";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Colon: return "Colon";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Equal: return "Equal";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::Power: return "Power";
        case TokenType::EqualEqual: return "EqualEqual";
        case TokenType::NotEqual: return "NotEqual";
        case TokenType::Less: return "Less";
        case TokenType::Greater: return "Greater";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::And: return "And";
        case TokenType::Or: return "Or";
        case TokenType::Not: return "Not";
        case TokenType::Question: return "Question";
        case TokenType::Arrow: return "Arrow";
        case TokenType::EventBind: return "EventBind";
        case TokenType::Ampersand: return "Ampersand";
        case TokenType::At: return "At";
        case TokenType::Hash: return "Hash";
        case TokenType::Dollar: return "Dollar";
        case TokenType::HtmlKeyword: return "HtmlKeyword";
        case TokenType::TextKeyword: return "TextKeyword";
        case TokenType::StyleKeyword: return "StyleKeyword";
        case TokenType::ScriptKeyword: return "ScriptKeyword";
        case TokenType::Template: return "Template";
        case TokenType::Custom: return "Custom";
        case TokenType::Origin: return "Origin";
        case TokenType::Import: return "Import";
        case TokenType::Namespace: return "Namespace";
        case TokenType::Configuration: return "Configuration";
        case TokenType::AtStyle: return "AtStyle";
        case TokenType::AtElement: return "AtElement";
        case TokenType::AtVar: return "AtVar";
        case TokenType::AtHtml: return "AtHtml";
        case TokenType::AtJavaScript: return "AtJavaScript";
        case TokenType::AtChtl: return "AtChtl";
        case TokenType::AtCJmod: return "AtCJmod";
        case TokenType::AtConfig: return "AtConfig";
        case TokenType::InheritKeyword: return "InheritKeyword";
        case TokenType::DeleteKeyword: return "DeleteKeyword";
        case TokenType::InsertKeyword: return "InsertKeyword";
        case TokenType::AfterKeyword: return "AfterKeyword";
        case TokenType::BeforeKeyword: return "BeforeKeyword";
        case TokenType::ReplaceKeyword: return "ReplaceKeyword";
        case TokenType::AtTopKeyword: return "AtTopKeyword";
        case TokenType::AtBottomKeyword: return "AtBottomKeyword";
        case TokenType::FromKeyword: return "FromKeyword";
        case TokenType::AsKeyword: return "AsKeyword";
        case TokenType::ExceptKeyword: return "ExceptKeyword";
        case TokenType::UseKeyword: return "UseKeyword";
        case TokenType::VirKeyword: return "VirKeyword";
        case TokenType::ListenKeyword: return "ListenKeyword";
        case TokenType::DelegateKeyword: return "DelegateKeyword";
        case TokenType::AnimateKeyword: return "AnimateKeyword";
        case TokenType::RouterKeyword: return "RouterKeyword";
        case TokenType::ScriptLoaderKeyword: return "ScriptLoaderKeyword";
        case TokenType::EnhancedSelector: return "EnhancedSelector";
        default: return "Unknown";
    }
}

} // namespace CHTL

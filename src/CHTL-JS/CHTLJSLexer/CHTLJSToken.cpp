#include "CHTLJSToken.h"

namespace CHTL {
namespace JS {

String JSToken::toString() const {
    return String(jsTokenTypeToString(type)) + "(\"" + value + "\") at " + position.toString();
}

const char* jsTokenTypeToString(JSTokenType type) {
    switch (type) {
        case JSTokenType::EndOfFile: return "EndOfFile";
        case JSTokenType::Unknown: return "Unknown";
        case JSTokenType::Identifier: return "Identifier";
        case JSTokenType::StringLiteral: return "StringLiteral";
        case JSTokenType::NumberLiteral: return "NumberLiteral";
        case JSTokenType::UnquotedLiteral: return "UnquotedLiteral";
        case JSTokenType::LeftBrace: return "LeftBrace";
        case JSTokenType::RightBrace: return "RightBrace";
        case JSTokenType::LeftBracket: return "LeftBracket";
        case JSTokenType::RightBracket: return "RightBracket";
        case JSTokenType::LeftParen: return "LeftParen";
        case JSTokenType::RightParen: return "RightParen";
        case JSTokenType::Semicolon: return "Semicolon";
        case JSTokenType::Colon: return "Colon";
        case JSTokenType::Comma: return "Comma";
        case JSTokenType::Dot: return "Dot";
        case JSTokenType::Equal: return "Equal";
        case JSTokenType::Plus: return "Plus";
        case JSTokenType::Minus: return "Minus";
        case JSTokenType::Star: return "Star";
        case JSTokenType::Slash: return "Slash";
        case JSTokenType::Percent: return "Percent";
        case JSTokenType::Arrow: return "Arrow";
        case JSTokenType::EventBind: return "EventBind";
        case JSTokenType::Ampersand: return "Ampersand";
        case JSTokenType::Question: return "Question";
        case JSTokenType::Vir: return "Vir";
        case JSTokenType::Listen: return "Listen";
        case JSTokenType::Delegate: return "Delegate";
        case JSTokenType::Animate: return "Animate";
        case JSTokenType::Router: return "Router";
        case JSTokenType::ScriptLoader: return "ScriptLoader";
        case JSTokenType::PrintMylove: return "PrintMylove";
        case JSTokenType::INeverAway: return "INeverAway";
        case JSTokenType::EnhancedSelector: return "EnhancedSelector";
        case JSTokenType::Const: return "Const";
        case JSTokenType::Let: return "Let";
        case JSTokenType::Var: return "Var";
        case JSTokenType::Function: return "Function";
        case JSTokenType::Return: return "Return";
        case JSTokenType::If: return "If";
        case JSTokenType::Else: return "Else";
        case JSTokenType::For: return "For";
        case JSTokenType::While: return "While";
        case JSTokenType::Break: return "Break";
        case JSTokenType::Continue: return "Continue";
        default: return "Unknown";
    }
}

} // namespace JS
} // namespace CHTL

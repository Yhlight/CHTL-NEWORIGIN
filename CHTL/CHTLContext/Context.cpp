#include "Context.h"

CHTLContext::CHTLContext() {
    initialize_default_keywords();
}

void CHTLContext::initialize_default_keywords() {
    keyword_to_token_map = {
        {"text", TokenType::TOKEN_KEYWORD_TEXT},
        {"style", TokenType::TOKEN_KEYWORD_STYLE},
        {"script", TokenType::TOKEN_KEYWORD_SCRIPT},
        {"Template", TokenType::TOKEN_KEYWORD_TEMPLATE},
        {"Custom", TokenType::TOKEN_KEYWORD_CUSTOM},
        {"Origin", TokenType::TOKEN_KEYWORD_ORIGIN},
        {"Import", TokenType::TOKEN_KEYWORD_IMPORT},
        {"Namespace", TokenType::TOKEN_KEYWORD_NAMESPACE},
        {"Configuration", TokenType::TOKEN_KEYWORD_CONFIGURATION}, // Added
        {"from", TokenType::TOKEN_KEYWORD_FROM},
        {"as", TokenType::TOKEN_KEYWORD_AS},
        {"use", TokenType::TOKEN_KEYWORD_USE},
        {"except", TokenType::TOKEN_KEYWORD_EXCEPT},
        {"delete", TokenType::TOKEN_KEYWORD_DELETE},
        {"insert", TokenType::TOKEN_KEYWORD_INSERT},
        {"inherit", TokenType::TOKEN_KEYWORD_INHERIT}
    };
}

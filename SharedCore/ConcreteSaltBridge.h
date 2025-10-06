#pragma once

#include "SaltBridge.h"
#include "../CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

class ConcreteSaltBridge : public SaltBridge {
public:
    std::string processScript(const std::string& raw_script) override {
        CHTLJS::CHTLJSLexer lexer(raw_script);
        std::vector<CHTLJS::Token> tokens;
        CHTLJS::Token token = lexer.getNextToken();
        while(token.type != CHTLJS::TokenType::TOKEN_EOF) {
            tokens.push_back(token);
            token = lexer.getNextToken();
        }

        CHTLJS::CHTLJSParser parser(tokens);
        std::stringstream final_script;

        while(!parser.isAtEnd()) {
            auto ast = parser.parse();
            if (ast) {
                CHTLJS::CHTLJSGenerator generator;
                final_script << generator.generate(ast);
            } else {
                CHTLJS::Token& current_token = parser.getCurrentToken();
                if (current_token.type == CHTLJS::TokenType::TOKEN_STRING_LITERAL) {
                    final_script << "\"" << current_token.lexeme << "\"";
                } else {
                    final_script << current_token.lexeme;
                }
                parser.advance();
            }
        }

        return final_script.str();
    }

    std::string processAnimation(const AnimationData& data) override {
        CHTLJS::CHTLJSGenerator generator;
        return generator.generateAnimation(data);
    }
};

}
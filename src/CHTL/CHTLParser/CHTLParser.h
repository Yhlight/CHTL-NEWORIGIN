#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../CHTLState/CHTLState.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../../Util/GlobalStylesheet/GlobalStylesheet.h"

class CHTLParser {
public:
    CHTLParser(const std::string& input);
    ~CHTLParser();
    void parse();
    void setState(std::unique_ptr<CHTLState> newState);
    Token consume();
    Token peek(size_t lookahead = 0);
    void putback(Token token);
    void setRawContentMode(bool enabled);

    // AST management
    void addNode(std::unique_ptr<BaseNode> node);
    void openScope(BaseNode* node);
    void closeScope();
    BaseNode* getCurrentScope();
    BaseNode* getParentScope();
    BaseNode* getRoot();
    std::unique_ptr<BaseNode> releaseRoot();
    GlobalStylesheet& getGlobalStylesheet();

private:
    std::unique_ptr<CHTLState> currentState;
    CHTLLexer lexer;
    std::vector<Token> tokenBuffer;
    std::unique_ptr<BaseNode> root;
    std::vector<BaseNode*> nodeStack;
    GlobalStylesheet stylesheet;
};
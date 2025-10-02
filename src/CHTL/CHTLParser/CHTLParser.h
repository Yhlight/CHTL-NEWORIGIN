#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../CHTLState/CHTLState.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/BaseNode.h"

class CHTLParser {
public:
    CHTLParser(const std::string& input);
    ~CHTLParser();
    void parse();
    void setState(std::unique_ptr<CHTLState> newState);
    Token getNextToken();

    // AST management
    void addNode(std::unique_ptr<BaseNode> node);
    void openScope(BaseNode* node);
    void closeScope();
    BaseNode* getCurrentScope();
    BaseNode* getRoot();

private:
    std::unique_ptr<CHTLState> currentState;
    CHTLLexer lexer;
    std::unique_ptr<BaseNode> root;
    std::vector<BaseNode*> nodeStack;
};
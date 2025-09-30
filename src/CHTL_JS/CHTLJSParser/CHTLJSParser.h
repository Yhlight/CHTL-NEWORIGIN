#ifndef CHTLJSPARSER_H
#define CHTLJSPARSER_H

#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <string>
#include <memory>
#include <vector>

class CHTLJSParser {
public:
    // The main entry point for parsing.
    std::unique_ptr<CHTLJSBaseNode> parse(const std::string& source);

private:
    std::vector<CHTLJSToken> tokens;
    size_t position = 0;

    CHTLJSToken currentToken();
    CHTLJSToken peekToken();
    CHTLJSToken consumeToken();
    void expectToken(CHTLJSTokenType type);

    // Main dispatching parse function
    std::unique_ptr<CHTLJSBaseNode> parseStatement();

    // Specific parsing functions for each construct
    std::unique_ptr<CHTLJSBaseNode> parseListen(const std::string& selector);
    std::unique_ptr<CHTLJSBaseNode> parseDelegate(const std::string& parent_selector);
    std::unique_ptr<CHTLJSBaseNode> parseAnimate();
    std::unique_ptr<CHTLJSBaseNode> parseRouter();
    std::unique_ptr<CHTLJSBaseNode> parseVir();
    std::unique_ptr<CHTLJSBaseNode> parseScriptLoader();
    std::unique_ptr<CHTLJSBaseNode> parseEnhancedSelector();
    std::unique_ptr<CHTLJSBaseNode> parseEventBinding(const std::string& selector);
};

#endif // CHTLJSPARSER_H
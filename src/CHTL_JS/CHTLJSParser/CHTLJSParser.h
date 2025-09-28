#pragma once

#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSNode/CHTLJSNode.h"
#include "../CHTLJSNode/ListenNode.h"
#include "../CHTLJSNode/EventBindingNode.h"
#include "../CHTLJSNode/DelegateNode.h"
#include "../CHTLJSNode/AnimateNode.h"
#include "../CHTLJSNode/ScriptLoaderNode.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include "../CHTLJSNode/VirtualObjectAccessNode.h"
#include "../CHTLJSNode/RouterNode.h"
#include <vector>
#include <memory>

// The CHTL JS Parser, responsible for building an AST from a token stream.
class CHTLJSParser {
public:
    explicit CHTLJSParser(std::vector<CHTLJSToken> tokens);
    std::vector<std::unique_ptr<CHTLJSNode>> parse();

private:
    std::vector<CHTLJSToken> tokens;
    size_t position = 0;

    CHTLJSToken currentToken() const;
    void advance();
    std::map<std::string, std::string> parseStyleMap();
    std::vector<AnimationKeyframe> parseKeyframeArray();

    std::unique_ptr<ListenNode> parseListenBlock();
    std::unique_ptr<EventBindingNode> parseEventBinding();
    std::unique_ptr<DelegateNode> parseDelegateBlock();
    std::unique_ptr<AnimateNode> parseAnimateBlock();
    std::unique_ptr<ScriptLoaderNode> parseScriptLoaderBlock();
    std::unique_ptr<VirtualObjectNode> parseVirtualObjectDeclaration();
    std::unique_ptr<RouterNode> parseRouterBlock();
};
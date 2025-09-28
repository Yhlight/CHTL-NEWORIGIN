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
#include "../CHTLJSNode/ReservedPlaceholderNode.h"
#include "../Util/Placeholder.h" // Include Placeholder
#include <vector>
#include <memory>

// The CHTL JS Parser, responsible for building an AST from a token stream.
class CHTLJSParser {
public:
    // The constructor now accepts the placeholder manager to resolve placeholders during parsing.
    explicit CHTLJSParser(std::vector<CHTLJSToken> tokens, const Placeholder& placeholder_manager);
    std::vector<std::unique_ptr<CHTLJSNode>> parse();

private:
    std::vector<CHTLJSToken> tokens;
    const Placeholder* placeholder_manager; // Pointer to the placeholder manager
    size_t position = 0;

    CHTLJSToken currentToken() const;
    void advance();
    std::string parseJsCodeBlock();
    std::map<std::string, std::string> parseStyleMap();
    std::vector<AnimationKeyframe> parseKeyframeArray();

    std::unique_ptr<ListenNode> parseListenBlock();
    std::unique_ptr<EventBindingNode> parseEventBinding();
    std::unique_ptr<DelegateNode> parseDelegateBlock();
    std::unique_ptr<AnimateNode> parseAnimateBlock();
    std::unique_ptr<ScriptLoaderNode> parseScriptLoaderBlock();
    std::unique_ptr<VirtualObjectNode> parseVirtualObjectDeclaration();
    std::unique_ptr<RouterNode> parseRouterBlock();
    std::unique_ptr<ReservedPlaceholderNode> parseReservedPlaceholder();
};
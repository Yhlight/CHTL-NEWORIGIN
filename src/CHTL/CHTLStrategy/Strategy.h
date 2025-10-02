/**
 * @file Strategy.h
 * @brief CHTL Strategy Pattern
 * 
 * Implements the Strategy Pattern for different parsing and generation strategies
 * This allows flexible handling of different CHTL syntax elements
 */

#ifndef CHTL_STRATEGY_H
#define CHTL_STRATEGY_H

#include <memory>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer/Token.h"

namespace CHTL {

class CHTLContext;
class BaseNode;

/**
 * @class ParseStrategy
 * @brief Base class for parsing strategies
 */
class ParseStrategy {
public:
    virtual ~ParseStrategy() = default;
    
    /**
     * @brief Parse tokens into AST node
     * @param tokens Vector of tokens to parse
     * @param context Compilation context
     * @return Parsed AST node
     */
    virtual std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) = 0;
    
    /**
     * @brief Check if this strategy can handle the token sequence
     */
    virtual bool canHandle(const std::vector<Token>& tokens) const = 0;
};

/**
 * @class ElementParseStrategy
 * @brief Strategy for parsing HTML elements
 */
class ElementParseStrategy : public ParseStrategy {
public:
    std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) override;
    
    bool canHandle(const std::vector<Token>& tokens) const override;
};

/**
 * @class StyleParseStrategy
 * @brief Strategy for parsing style blocks
 */
class StyleParseStrategy : public ParseStrategy {
public:
    std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) override;
    
    bool canHandle(const std::vector<Token>& tokens) const override;
};

/**
 * @class ScriptParseStrategy
 * @brief Strategy for parsing script blocks
 * Note: Delegates to CHTL JS compiler via salt bridge
 */
class ScriptParseStrategy : public ParseStrategy {
public:
    std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) override;
    
    bool canHandle(const std::vector<Token>& tokens) const override;
};

/**
 * @class TextParseStrategy
 * @brief Strategy for parsing text nodes
 */
class TextParseStrategy : public ParseStrategy {
public:
    std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) override;
    
    bool canHandle(const std::vector<Token>& tokens) const override;
};

/**
 * @class TemplateParseStrategy
 * @brief Strategy for parsing template definitions
 */
class TemplateParseStrategy : public ParseStrategy {
public:
    std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) override;
    
    bool canHandle(const std::vector<Token>& tokens) const override;
};

/**
 * @class CustomParseStrategy
 * @brief Strategy for parsing custom definitions
 */
class CustomParseStrategy : public ParseStrategy {
public:
    std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) override;
    
    bool canHandle(const std::vector<Token>& tokens) const override;
};

/**
 * @class CommentParseStrategy
 * @brief Strategy for parsing comments
 */
class CommentParseStrategy : public ParseStrategy {
public:
    std::shared_ptr<BaseNode> parse(
        const std::vector<Token>& tokens,
        CHTLContext& context
    ) override;
    
    bool canHandle(const std::vector<Token>& tokens) const override;
};

/**
 * @class GenerationStrategy
 * @brief Base class for code generation strategies
 */
class GenerationStrategy {
public:
    virtual ~GenerationStrategy() = default;
    
    /**
     * @brief Generate output from AST node
     * @param node AST node to generate from
     * @param context Compilation context
     * @return Generated code string
     */
    virtual std::string generate(
        const std::shared_ptr<BaseNode>& node,
        CHTLContext& context
    ) = 0;
};

/**
 * @class HTMLGenerationStrategy
 * @brief Strategy for generating HTML output
 */
class HTMLGenerationStrategy : public GenerationStrategy {
public:
    std::string generate(
        const std::shared_ptr<BaseNode>& node,
        CHTLContext& context
    ) override;
};

/**
 * @class CSSGenerationStrategy
 * @brief Strategy for generating CSS output
 */
class CSSGenerationStrategy : public GenerationStrategy {
public:
    std::string generate(
        const std::shared_ptr<BaseNode>& node,
        CHTLContext& context
    ) override;
};

/**
 * @class JSGenerationStrategy
 * @brief Strategy for generating JavaScript output
 */
class JSGenerationStrategy : public GenerationStrategy {
public:
    std::string generate(
        const std::shared_ptr<BaseNode>& node,
        CHTLContext& context
    ) override;
};

/**
 * @class StrategySelector
 * @brief Selects appropriate strategy based on tokens or node type
 */
class StrategySelector {
public:
    /**
     * @brief Select parsing strategy for token sequence
     */
    static std::shared_ptr<ParseStrategy> selectParseStrategy(
        const std::vector<Token>& tokens
    );
    
    /**
     * @brief Select generation strategy for node type
     */
    static std::shared_ptr<GenerationStrategy> selectGenerationStrategy(
        const std::string& nodeType
    );
};

} // namespace CHTL

#endif // CHTL_STRATEGY_H

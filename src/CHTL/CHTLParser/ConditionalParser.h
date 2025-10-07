/**
 * @file ConditionalParser.h
 * @brief Parser for conditional rendering blocks (if/else if/else)
 * 
 * Part of CHTL Committee 3rd Period: Element Behaviors and Conditional Rendering
 */

#ifndef CHTL_CONDITIONAL_PARSER_H
#define CHTL_CONDITIONAL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/ConditionalNode.h"
#include "../../Common.h"
#include <memory>

namespace CHTL {

/**
 * @brief Parser for conditional rendering blocks
 * 
 * Handles parsing of:
 * - if { condition: ..., property: value, }
 * - else if { condition: ..., property: value, }
 * - else { property: value, }
 */
class ConditionalParser {
public:
    /**
     * @brief Constructor
     * @param tokens Reference to token stream
     */
    explicit ConditionalParser(const Vector<Token>& tokens);
    
    /**
     * @brief Parse an if block
     * @param currentPos Current position in token stream
     * @return Conditional node or nullptr if parse fails
     */
    SharedPtr<ConditionalNode> parseIfBlock(size_t& currentPos);
    
    /**
     * @brief Parse an else-if block
     * @param currentPos Current position in token stream
     * @return Conditional node or nullptr if parse fails
     */
    SharedPtr<ConditionalNode> parseElseIfBlock(size_t& currentPos);
    
    /**
     * @brief Parse an else block
     * @param currentPos Current position in token stream
     * @return Conditional node or nullptr if parse fails
     */
    SharedPtr<ConditionalNode> parseElseBlock(size_t& currentPos);
    
    /**
     * @brief Parse complete conditional chain (if + else if + else)
     * @param currentPos Current position in token stream
     * @return Root conditional node with chain
     */
    SharedPtr<ConditionalNode> parseConditionalChain(size_t& currentPos);
    
    /**
     * @brief Check if current position is at 'if' keyword
     * @param pos Position to check
     * @return true if at 'if' keyword
     */
    bool isAtIfKeyword(size_t pos) const;
    
    /**
     * @brief Check if current position is at 'else' keyword
     * @param pos Position to check
     * @return true if at 'else' keyword
     */
    bool isAtElseKeyword(size_t pos) const;
    
    /**
     * @brief Check if current position is at 'else if' keywords
     * @param pos Position to check
     * @return true if at 'else if' keywords
     */
    bool isAtElseIfKeywords(size_t pos) const;

private:
    /**
     * @brief Parse condition expression
     * @param currentPos Current position in token stream
     * @return Condition string
     */
    String parseCondition(size_t& currentPos);
    
    /**
     * @brief Parse CSS properties in conditional block
     * @param currentPos Current position in token stream
     * @param node Conditional node to add properties to
     */
    void parseConditionalProperties(size_t& currentPos, SharedPtr<ConditionalNode>& node);
    
    /**
     * @brief Parse a single CSS property
     * @param currentPos Current position in token stream
     * @return Pair of (property, value) or empty pair if failed
     */
    Pair<String, String> parseProperty(size_t& currentPos);
    
    /**
     * @brief Expect a specific token type
     * @param currentPos Current position in token stream
     * @param type Expected token type
     * @param errorMsg Error message if token doesn't match
     * @return true if token matches, false otherwise
     */
    bool expectToken(size_t& currentPos, TokenType type, const String& errorMsg = "");
    
    /**
     * @brief Expect a specific keyword
     * @param currentPos Current position in token stream
     * @param keyword Expected keyword
     * @param errorMsg Error message if keyword doesn't match
     * @return true if keyword matches, false otherwise
     */
    bool expectKeyword(size_t& currentPos, const String& keyword, const String& errorMsg = "");
    
    /**
     * @brief Match a token type (doesn't consume if doesn't match)
     * @param pos Position to check
     * @param type Token type to match
     * @return true if matches
     */
    bool matchToken(size_t pos, TokenType type) const;
    
    /**
     * @brief Match a keyword (doesn't consume if doesn't match)
     * @param pos Position to check
     * @param keyword Keyword to match
     * @return true if matches
     */
    bool matchKeyword(size_t pos, const String& keyword) const;
    
    /**
     * @brief Get token at position
     * @param pos Position
     * @return Token or Token::Invalid if out of bounds
     */
    const Token& getToken(size_t pos) const;
    
    /**
     * @brief Check if position is valid
     * @param pos Position to check
     * @return true if valid
     */
    bool isValidPosition(size_t pos) const;
    
    /**
     * @brief Detect if condition contains {{}} enhanced selectors
     * @param condition The condition string
     * @return true if dynamic (contains {{}})
     */
    bool isDynamicCondition(const String& condition) const;
    
    /**
     * @brief Skip whitespace and comments
     * @param currentPos Current position in token stream
     */
    void skipWhitespaceAndComments(size_t& currentPos);

private:
    /// Token stream
    const Vector<Token>& tokens_;
    
    /// Invalid token for out-of-bounds access
    static const Token invalidToken_;
};

} // namespace CHTL

#endif // CHTL_CONDITIONAL_PARSER_H

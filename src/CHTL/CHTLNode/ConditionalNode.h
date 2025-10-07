/**
 * @file ConditionalNode.h
 * @brief AST node for conditional rendering (if/else if/else blocks)
 * 
 * Part of CHTL Committee 3rd Period: Element Behaviors and Conditional Rendering
 * Implements if/else if/else conditional blocks as defined in CHTL.md
 */

#ifndef CHTL_CONDITIONAL_NODE_H
#define CHTL_CONDITIONAL_NODE_H

#include "BaseNode.h"
#include "../../Common.h"
#include <memory>
#include <vector>
#include <utility>

namespace CHTL {

/**
 * @brief Represents a conditional block (if/else if/else)
 * 
 * Example CHTL code:
 * ```chtl
 * div
 * {
 *     if
 *     {
 *         condition: width > 100px,
 *         display: block,
 *         color: red,
 *     }
 *     else if
 *     {
 *         condition: width > 50px,
 *         display: inline-block,
 *     }
 *     else
 *     {
 *         display: none,
 *     }
 * }
 * ```
 */
class ConditionalNode : public BaseNode {
public:
    /**
     * @brief Constructor
     */
    ConditionalNode();
    
    /**
     * @brief Destructor
     */
    ~ConditionalNode() override = default;
    
    /**
     * @brief Get node type name
     * @return Node type string
     */
    String getTypeName() const { return "ConditionalNode"; }
    
    /**
     * @brief Accept visitor
     * @param visitor The visitor
     */
    void accept(NodeVisitor& visitor) override;
    
    /**
     * @brief Clone this node
     * @return Cloned node
     */
    SharedPtr<BaseNode> clone() const override;
    
    /**
     * @brief Check if this is a dynamic conditional (contains {{}})
     * @return true if dynamic, false if static
     */
    bool isDynamic() const { return isDynamic_; }
    
    /**
     * @brief Set dynamic flag
     * @param dynamic Whether this conditional uses {{}} selectors
     */
    void setDynamic(bool dynamic) { isDynamic_ = dynamic; }
    
    /**
     * @brief Get the condition expression
     * @return Condition string (e.g., "width > 100px")
     */
    String getCondition() const { return condition_; }
    
    /**
     * @brief Set the condition expression
     * @param condition The condition string
     */
    void setCondition(const String& condition) { condition_ = condition; }
    
    /**
     * @brief Get CSS properties to apply when condition is true
     * @return Vector of (property, value) pairs
     */
    const Vector<Pair<String, String>>& getStyles() const { return styles_; }
    
    /**
     * @brief Add a CSS property to this conditional block
     * @param property Property name (e.g., "display")
     * @param value Property value (e.g., "block")
     */
    void addStyle(const String& property, const String& value);
    
    /**
     * @brief Get else-if blocks
     * @return Vector of else-if conditional nodes
     */
    const Vector<SharedPtr<ConditionalNode>>& getElseIfBlocks() const { return elseIfBlocks_; }
    
    /**
     * @brief Add an else-if block
     * @param elseIfNode The else-if conditional node
     */
    void addElseIfBlock(SharedPtr<ConditionalNode> elseIfNode);
    
    /**
     * @brief Get else block
     * @return Else block node or nullptr if no else
     */
    SharedPtr<ConditionalNode> getElseBlock() const { return elseBlock_; }
    
    /**
     * @brief Set else block
     * @param elseNode The else conditional node
     */
    void setElseBlock(SharedPtr<ConditionalNode> elseNode);
    
    /**
     * @brief Check if this is an if block (vs else-if or else)
     * @return true if this is the main if block
     */
    bool isIfBlock() const { return blockType_ == BlockType::If; }
    
    /**
     * @brief Check if this is an else-if block
     * @return true if this is an else-if block
     */
    bool isElseIfBlock() const { return blockType_ == BlockType::ElseIf; }
    
    /**
     * @brief Check if this is an else block
     * @return true if this is an else block
     */
    bool isElseBlock() const { return blockType_ == BlockType::Else; }
    
    /**
     * @brief Block type enumeration
     */
    enum class BlockType {
        If,      // Main if block
        ElseIf,  // Else if block
        Else     // Else block
    };
    
    /**
     * @brief Get block type
     * @return The block type
     */
    BlockType getBlockType() const { return blockType_; }
    
    /**
     * @brief Set block type
     * @param type The block type
     */
    void setBlockType(BlockType type) { blockType_ = type; }
    
    /**
     * @brief Extract enhanced selectors from condition
     * @return Vector of {{selector}} strings found in condition
     */
    Vector<String> extractEnhancedSelectors() const;
    
    /**
     * @brief Check if condition contains arithmetic operations
     * @return true if condition has +, -, *, /, %, **
     */
    bool hasArithmeticInCondition() const;
    
    /**
     * @brief Check if condition contains logical operators
     * @return true if condition has &&, ||
     */
    bool hasLogicalOperators() const;
    
    /**
     * @brief Get parent element selector (for context)
     * @return Parent element's selector or empty string
     */
    String getParentSelector() const { return parentSelector_; }
    
    /**
     * @brief Set parent element selector
     * @param selector The parent selector
     */
    void setParentSelector(const String& selector) { parentSelector_ = selector; }

private:
    /// Condition expression (e.g., "width > 100px", "{{html}}->width < 500px")
    String condition_;
    
    /// CSS properties to apply when condition is true
    Vector<Pair<String, String>> styles_;
    
    /// Chain of else-if blocks
    Vector<SharedPtr<ConditionalNode>> elseIfBlocks_;
    
    /// Final else block (optional)
    SharedPtr<ConditionalNode> elseBlock_;
    
    /// Whether this uses dynamic {{}} selectors
    bool isDynamic_;
    
    /// Block type (if, else if, or else)
    BlockType blockType_;
    
    /// Parent element selector for context
    String parentSelector_;
};

/**
 * @brief Utility functions for conditional nodes
 */
namespace ConditionalUtils {
    /**
     * @brief Check if a condition string contains enhanced selectors
     * @param condition The condition string
     * @return true if contains {{...}}
     */
    bool containsEnhancedSelector(const String& condition);
    
    /**
     * @brief Extract all {{selector}} patterns from condition
     * @param condition The condition string
     * @return Vector of selector strings (without {{}})
     */
    Vector<String> extractEnhancedSelectors(const String& condition);
    
    /**
     * @brief Check if condition is valid
     * @param condition The condition string
     * @return true if valid, false otherwise
     */
    bool isValidCondition(const String& condition);
    
    /**
     * @brief Parse condition into parts (left operand, operator, right operand)
     * @param condition The condition string
     * @return Tuple of (left, operator, right)
     */
    Tuple<String, String, String> parseCondition(const String& condition);
}

} // namespace CHTL

#endif // CHTL_CONDITIONAL_NODE_H

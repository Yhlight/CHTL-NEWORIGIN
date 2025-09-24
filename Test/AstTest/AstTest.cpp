#include <iostream>
#include <cassert>
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"

using namespace CHTL;

int main() {
    std::cout << "Running CHTL AST Tests..." << std::endl;
    
    // Test BaseNode creation
    auto baseNode = std::make_unique<BaseNode>(NodeType::BASE);
    assert(baseNode->getType() == NodeType::BASE);
    assert(baseNode->getTypeName() == "BaseNode");
    std::cout << "✓ BaseNode creation test passed" << std::endl;
    
    // Test ElementNode creation
    auto elementNode = std::make_unique<ElementNode>("div");
    assert(elementNode->getName() == "div");
    assert(elementNode->isValid());
    std::cout << "✓ ElementNode creation test passed" << std::endl;
    
    // Test node properties
    baseNode->setProperty("test", "value");
    assert(baseNode->hasProperty("test"));
    assert(baseNode->getProperty("test") == "value");
    std::cout << "✓ Node properties test passed" << std::endl;
    
    // Test node children
    baseNode->addChild(std::move(elementNode));
    assert(baseNode->getChildren().size() == 1);
    std::cout << "✓ Node children test passed" << std::endl;
    
    // Test node cloning
    auto clonedNode = baseNode->clone();
    assert(clonedNode->getType() == baseNode->getType());
    assert(clonedNode->hasProperty("test"));
    std::cout << "✓ Node cloning test passed" << std::endl;
    
    // Test node validation
    auto invalidElement = std::make_unique<ElementNode>("");
    assert(!invalidElement->isValid());
    std::cout << "✓ Node validation test passed" << std::endl;
    
    std::cout << "All AST tests passed!" << std::endl;
    return 0;
}
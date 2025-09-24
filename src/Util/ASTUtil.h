#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include <vector>
#include <memory>
#include <string>
#include "Selector.h" // Use the new Selector struct

// Searches the provided list of nodes and their descendants for the first
// element that matches the complex selector.
const ElementNode* findNodeBySelector(const std::vector<std::unique_ptr<BaseNode>>& nodes, const Selector& selector);

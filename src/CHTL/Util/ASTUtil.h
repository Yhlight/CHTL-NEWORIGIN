#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include <vector>
#include <memory>
#include <string>

// Searches the provided list of nodes and their descendants for the first
// element that matches the simple selector (e.g., "#id", ".class").
const ElementNode* findNodeBySelector(const std::vector<std::unique_ptr<BaseNode>>& nodes, const std::string& selector);

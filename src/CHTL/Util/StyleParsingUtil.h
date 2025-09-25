#pragma once

#include <string>

class Parser; // Forward declaration

// A utility function to parse the body of a CSS rule block '{...}'.
// This is used by both local and global style block states.
std::string parseCssRuleBlock(Parser& parser);
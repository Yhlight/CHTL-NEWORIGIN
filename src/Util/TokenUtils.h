#pragma once

#include "../Scanner/Tokenizer.h"
#include <string>
#include <vector>

// Finds the index of the matching '}' token for an opening '{' token.
size_t findMatchingBraceToken(const std::vector<Token>& tokens, size_t start_index);

// Finds the index of the next token that is not whitespace or a comment.
size_t nextMeaningfulToken(const std::vector<Token>& tokens, size_t current_index);
#include "CHTLJSPreprocessor.h"
#include "../Util/TokenUtils.h"
#include <iostream>

// This pass finds iNeverAway declarations, stores them, and removes them from the token stream.
void CHTLJSPreprocessor::findDeclarations(std::vector<Token>& tokens) {
    std::vector<Token> processed_tokens;
    size_t i = 0;
    while (i < tokens.size()) {
        bool vir_block_processed = false;
        if (tokens[i].type == TokenType::VIR_KEYWORD) {
            size_t var_name_idx = nextMeaningfulToken(tokens, i);
            size_t equals_idx = nextMeaningfulToken(tokens, var_name_idx);
            size_t value_idx = nextMeaningfulToken(tokens, equals_idx);

            if (value_idx != std::string::npos && tokens[var_name_idx].type == TokenType::IDENTIFIER && tokens[equals_idx].text == "=") {
                std::string var_name = tokens[var_name_idx].text;

                if (tokens[value_idx].text == "iNeverAway") {
                    size_t open_brace_idx = nextMeaningfulToken(tokens, value_idx);
                    if (open_brace_idx != std::string::npos && tokens[open_brace_idx].text == "{") {
                        size_t close_brace_idx = findMatchingBraceToken(tokens, open_brace_idx);
                        if (close_brace_idx != std::string::npos) {

                            size_t content_pos = open_brace_idx + 1;
                            while (content_pos < close_brace_idx) {
                                size_t sig_start = nextMeaningfulToken(tokens, content_pos - 1);
                                if (sig_start >= close_brace_idx) break;

                                size_t sig_end = sig_start;
                                // Consume tokens until we hit the colon
                                while(sig_end < close_brace_idx && tokens[sig_end].text != ":") sig_end++;
                                if(sig_end >= close_brace_idx) break;

                                std::string signature;
                                for(size_t k = sig_start; k < sig_end; k++) {
                                    if(tokens[k].type != TokenType::WHITESPACE) signature += tokens[k].text;
                                }

                                size_t func_start = nextMeaningfulToken(tokens, sig_end);
                                if (func_start >= close_brace_idx) break;

                                size_t func_end = func_start;
                                int brace_balance = 0;
                                int paren_balance = 0;
                                for (size_t k = func_start; k < close_brace_idx; ++k) {
                                    if (tokens[k].text == "{") brace_balance++;
                                    else if (tokens[k].text == "}") brace_balance--;
                                    else if (tokens[k].text == "(") paren_balance++;
                                    else if (tokens[k].text == ")") paren_balance--;

                                    if (brace_balance == 0 && paren_balance == 0 && tokens[k].text == ",") {
                                        func_end = k - 1;
                                        break;
                                    }
                                    func_end = k;
                                }

                                std::vector<Token> func_tokens(tokens.begin() + func_start, tokens.begin() + func_end + 1);
                                ineveraway_map[var_name][signature] = func_tokens;
                                content_pos = func_end + 2;
                            }

                            i = close_brace_idx;
                            size_t next = nextMeaningfulToken(tokens, i);
                            if(next != std::string::npos && tokens[next].text == ";") i = next;
                            vir_block_processed = true;
                        }
                    }
                }
            }
        }

        if (!vir_block_processed) {
            processed_tokens.push_back(tokens[i]);
        }
        i++;
    }
    tokens = processed_tokens;
}

// This pass finds accesses to iNeverAway objects and replaces them with function calls.
void CHTLJSPreprocessor::substituteAccesses(std::vector<Token>& tokens, CHTLJSCompiler& compiler) {
    std::vector<Token> final_tokens;
    std::string functions_to_inject;
    size_t i = 0;
    while(i < tokens.size()) {
        if (i + 1 < tokens.size() && tokens[i].type == TokenType::IDENTIFIER &&
            tokens[i+1].type == TokenType::CHTL_OPERATOR) {

            std::string var_name = tokens[i].text;
            size_t sig_start = nextMeaningfulToken(tokens, i + 1);
            if(sig_start == std::string::npos) {
                final_tokens.push_back(tokens[i]);
                i++;
                continue;
            }

            size_t sig_end = sig_start;
            // Consume a sequence of IDENTIFIER and < > symbols
            while(sig_end < tokens.size() &&
                  (tokens[sig_end].type == TokenType::IDENTIFIER ||
                   (tokens[sig_end].type == TokenType::SYMBOL && (tokens[sig_end].text == "<" || tokens[sig_end].text == ">"))))
            {
                sig_end++;
            }

            std::string signature;
            for(size_t k = sig_start; k < sig_end; ++k) signature += tokens[k].text;

            if (ineveraway_map.count(var_name) && ineveraway_map.at(var_name).count(signature)) {
                auto func_tokens = ineveraway_map.at(var_name).at(signature);
                std::string global_func_name = "__chtl_global_func_" + std::to_string(global_func_counter++);

                // Recursively compile the body of the iNeverAway function
                std::string compiled_body = compiler.compile(func_tokens);

                std::string func_def = "function " + global_func_name;
                // Simplified body extraction, assuming "function(...) { ... }"
                size_t first_paren = compiled_body.find('(');
                if (first_paren != std::string::npos) {
                    func_def += compiled_body.substr(first_paren);
                } else {
                    func_def += "() " + compiled_body; // Failsafe
                }
                functions_to_inject += func_def + "\n";

                final_tokens.push_back({TokenType::IDENTIFIER, global_func_name});
                i = sig_end;
            } else {
                final_tokens.push_back(tokens[i]);
                i++;
            }
        } else {
            final_tokens.push_back(tokens[i]);
            i++;
        }
    }

    if (!functions_to_inject.empty()) {
        final_tokens.insert(final_tokens.begin(), {TokenType::JS_BLOCK, functions_to_inject});
    }
    tokens = final_tokens;
}

void CHTLJSPreprocessor::process(std::vector<Token>& tokens, CHTLJSCompiler& compiler) {
    ineveraway_map.clear();
    global_func_counter = 0;
    findDeclarations(tokens);
    substituteAccesses(tokens, compiler);
}
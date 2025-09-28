#include "CHTLJSCompiler.h"
#include "../CHTLLexer/CHTLJSLexer.h"
#include "../CHTLJSParser/CHTLJSParser.h"
#include "../CHTLJSGenerator/CHTLJSGenerator.h"
#include "../CHTLJSNode/VirtualObjectNode.h"
#include <vector>
#include <algorithm>

// This is the final, robust implementation of granular_scan.
// It correctly identifies CHTL JS constructs as whole units, including their blocks,
// and replaces all other code with placeholders, as per the CHTL.md design.
std::string CHTLJSCompiler::granular_scan(const std::string& content) {
    std::string result;
    std::string js_buffer;
    placeholder_manager.placeholder_map.clear();

    const std::vector<std::string> block_keywords = {
        "Listen", "Delegate", "Animate", "Router", "ScriptLoader"
    };
    const std::vector<std::string> other_tokens = {
        "->", "&->", "$", "util", "Vir"
    };

    for (size_t i = 0; i < content.length();) {
        bool token_found = false;

        // Handle {{...}} selectors as a whole unit.
        if (content.substr(i, 2) == "{{") {
            if (!js_buffer.empty()) {
                result += placeholder_manager.generate(PlaceholderType::JS, js_buffer);
                js_buffer.clear();
            }
            size_t end_pos = content.find("}}", i);
            if (end_pos != std::string::npos) {
                result += content.substr(i, end_pos - i + 2);
                i = end_pos + 2;
                continue;
            }
        }

        // Handle block-starting keywords.
        for (const auto& keyword : block_keywords) {
            if (content.substr(i, keyword.length()) == keyword) {
                if (!js_buffer.empty()) {
                    result += placeholder_manager.generate(PlaceholderType::JS, js_buffer);
                    js_buffer.clear();
                }

                size_t block_start_search = i + keyword.length();
                while(block_start_search < content.length() && isspace(content[block_start_search])) {
                    block_start_search++;
                }

                if (block_start_search < content.length() && content[block_start_search] == '{') {
                    int brace_level = 1;
                    size_t block_end = block_start_search + 1;
                    while (block_end < content.length() && brace_level > 0) {
                        if (content[block_end] == '{') brace_level++;
                        else if (content[block_end] == '}') brace_level--;
                        block_end++;
                    }
                    if (brace_level == 0) {
                        result += content.substr(i, block_end - i);
                        i = block_end;
                        token_found = true;
                        break;
                    }
                }
            }
        }
        if (token_found) continue;

        // Handle other CHTL JS tokens.
        for (const auto& token : other_tokens) {
            if (content.substr(i, token.length()) == token) {
                if (!js_buffer.empty()) {
                    result += placeholder_manager.generate(PlaceholderType::JS, js_buffer);
                    js_buffer.clear();
                }
                result += token;
                i += token.length();
                token_found = true;
                break;
            }
        }
        if (token_found) continue;

        js_buffer += content[i];
        i++;
    }

    if (!js_buffer.empty()) {
        result += placeholder_manager.generate(PlaceholderType::JS, js_buffer);
    }

    return result;
}

// The parse method now creates the parser with the placeholder manager.
std::vector<std::unique_ptr<CHTLJSNode>> CHTLJSCompiler::parse(const std::string& chtl_js_source) {
    std::string scanned_source = granular_scan(chtl_js_source);
    auto lexer = createLexer(scanned_source);
    auto tokens = lexer.tokenize();
    CHTLJSParser parser(std::move(tokens), placeholder_manager);
    return parser.parse();
}

std::string CHTLJSCompiler::generate(std::vector<std::unique_ptr<CHTLJSNode>>& ast) {
    virtualObjectManager.clear();
    std::vector<std::unique_ptr<CHTLJSNode>> generator_ast;

    for (auto& node : ast) {
        if (node && node->getType() == CHTLJSNodeType::VirtualObject) {
            auto virNode = std::unique_ptr<VirtualObjectNode>(static_cast<VirtualObjectNode*>(node.release()));
            virtualObjectManager.registerVirtualObject(virNode->name, std::move(virNode));
        }
    }

    for (auto& node : ast) {
        if (node) {
            generator_ast.push_back(std::move(node));
        }
    }

    return generator.generate(generator_ast, virtualObjectManager, placeholder_manager);
}

CHTLJSLexer CHTLJSCompiler::createLexer(const std::string& source) {
    return CHTLJSLexer(source);
}
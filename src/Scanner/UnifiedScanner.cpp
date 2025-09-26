#include "UnifiedScanner.h"
#include <cctype>
#include <algorithm>
#include <string>

namespace {
// Helper to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// This scanner is designed to find and extract top-level style blocks
// and all script blocks, separating the latter into JS and CHTL_JS.
class FinalScanner {
public:
    FinalScanner(const std::string& source) : src(source), last_flush_pos(0) {}

    std::vector<CodeFragment> scan() {
        size_t search_pos = 0;

        while (search_pos < src.length()) {
            size_t style_pos = src.find("style", search_pos);
            size_t script_pos = src.find("script", search_pos);
            size_t keyword_pos = std::min(style_pos, script_pos);

            if (keyword_pos == std::string::npos) {
                break; // No more blocks to process
            }

            const std::string& keyword = (keyword_pos == style_pos) ? "style" : "script";
            bool is_whole_word = (keyword_pos == 0 || !isalnum(src[keyword_pos - 1])) &&
                                 (keyword_pos + keyword.length() >= src.length() || !isalnum(src[keyword_pos + keyword.length()]));

            if (!is_whole_word) {
                search_pos = keyword_pos + 1;
                continue;
            }

            size_t open_brace_pos = src.find('{', keyword_pos + keyword.length());
            bool valid_block = true;
            if (open_brace_pos == std::string::npos) {
                valid_block = false;
            } else {
                for (size_t i = keyword_pos + keyword.length(); i < open_brace_pos; ++i) {
                    if (!isspace(src[i])) {
                        valid_block = false;
                        break;
                    }
                }
            }

            if (!valid_block) {
                search_pos = keyword_pos + 1;
                continue;
            }

            int level_before = 0;
            for (size_t i = 0; i < keyword_pos; ++i) {
                if (src[i] == '{') level_before++;
                else if (src[i] == '}') level_before--;
            }

            bool is_top_level_style = (keyword == "style" && level_before == 0);
            bool is_any_script = (keyword == "script");

            if (is_top_level_style || is_any_script) {
                flush_chtml(keyword_pos);

                size_t content_start = open_brace_pos + 1;
                size_t current_pos = content_start;
                int block_brace_level = 1;
                while (current_pos < src.length() && block_brace_level > 0) {
                    if (src[current_pos] == '{') block_brace_level++;
                    else if (src[current_pos] == '}') block_brace_level--;
                    current_pos++;
                }

                if (is_top_level_style) {
                    fragments.push_back({FragmentType::CSS, src.substr(content_start, current_pos - content_start - 1)});
                } else {
                    scanScriptContent(src.substr(content_start, current_pos - content_start - 1));
                }

                last_flush_pos = current_pos;
                search_pos = current_pos;
            } else {
                search_pos = keyword_pos + 1;
            }
        }

        flush_chtml(src.length());
        if (fragments.empty() && !src.empty()) {
            fragments.push_back({FragmentType::CHTL, src});
        }
        return fragments;
    }

private:
    const std::string& src;
    size_t last_flush_pos;
    std::vector<CodeFragment> fragments;

    void flush_chtml(size_t end_pos) {
        if (end_pos > last_flush_pos) {
            std::string content = src.substr(last_flush_pos, end_pos - last_flush_pos);
            if (!trim(content).empty()) {
                fragments.push_back({FragmentType::CHTL, content});
            }
        }
    }

    void scanScriptContent(const std::string& content) {
        size_t current_pos = 0;
        size_t last_js_flush = 0;
        while (current_pos < content.length()) {
            size_t open_brace_pos = content.find("{{", current_pos);
            size_t dollar_pos = content.find('$', current_pos);

            if (dollar_pos != std::string::npos) {
                if (dollar_pos + 1 >= content.length() || content.find('$', dollar_pos + 1) == std::string::npos) {
                    dollar_pos = std::string::npos;
                }
            }

            size_t chtl_js_pos = std::min(open_brace_pos, dollar_pos);

            if (chtl_js_pos == std::string::npos) {
                break;
            }

            if (chtl_js_pos > last_js_flush) {
                fragments.push_back({FragmentType::JS, content.substr(last_js_flush, chtl_js_pos - last_js_flush)});
            }

            size_t chtl_js_end = std::string::npos;
            if (chtl_js_pos == open_brace_pos) {
                chtl_js_end = content.find("}}", chtl_js_pos);
                if (chtl_js_end != std::string::npos) chtl_js_end += 2;
            } else {
                chtl_js_end = content.find('$', chtl_js_pos + 1);
                if (chtl_js_end != std::string::npos) chtl_js_end += 1;
            }

            if (chtl_js_end == std::string::npos) chtl_js_end = content.length();

            fragments.push_back({FragmentType::CHTL_JS, content.substr(chtl_js_pos, chtl_js_end - chtl_js_pos)});
            current_pos = chtl_js_end;
            last_js_flush = current_pos;
        }

        if (last_js_flush < content.length()) {
            fragments.push_back({FragmentType::JS, content.substr(last_js_flush)});
        }
    }
};

} // anonymous namespace

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& source) {
    FinalScanner scanner(source);
    return scanner.scan();
}
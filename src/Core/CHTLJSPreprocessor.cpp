#include "CHTLJSPreprocessor.h"
#include "../Util/StringUtils.h" // For findMatchingBrace
#include <regex>

void CHTLJSPreprocessor::findDeclarations(std::string& source) {
    size_t pos = 0;
    while ((pos = source.find("Vir ", pos)) != std::string::npos) {
        size_t var_start = pos + 4;
        var_start = source.find_first_not_of(" \t\n\r", var_start);
        if (var_start == std::string::npos) break;

        size_t var_end = source.find_first_of(" \t\n\r=", var_start);
        if (var_end == std::string::npos) break;
        std::string var_name = source.substr(var_start, var_end - var_start);

        size_t equals_pos = source.find('=', var_end);
        if (equals_pos == std::string::npos) break;

        size_t value_start = source.find_first_not_of(" \t\n\r", equals_pos + 1);
        if (value_start == std::string::npos) break;

        size_t open_brace_pos = source.find('{', value_start);
        if (open_brace_pos == std::string::npos) break;

        size_t close_brace_pos = findMatchingBrace(source, open_brace_pos);
        if (close_brace_pos == std::string::npos) break;

        size_t semi_pos = source.find(';', close_brace_pos);
        if (semi_pos == std::string::npos) break;

        std::string func_block = source.substr(value_start, close_brace_pos - value_start + 1);
        vir_map[var_name] = func_block;

        source.erase(pos, semi_pos - pos + 1);
    }
}

void CHTLJSPreprocessor::substituteAccesses(std::string& source) {
    std::regex access_regex(R"(([\w\d_]+)\s*->\s*([\w\d_]+))");
    std::string result;
    size_t last_pos = 0;
    auto it = std::sregex_iterator(source.begin(), source.end(), access_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::smatch match = *it;
        result.append(source, last_pos, match.position() - last_pos);

        std::string var_name = match[1].str();
        std::string prop_name = match[2].str();

        if (vir_map.count(var_name)) {
            std::string func_body = vir_map.at(var_name);
            std::string content = func_body.substr(func_body.find('{') + 1);
            content.pop_back();

            size_t prop_pos = content.find(prop_name);
            if (prop_pos != std::string::npos) {
                size_t colon_pos = content.find(':', prop_pos);
                size_t value_start = content.find_first_not_of(" \t\n\r", colon_pos + 1);
                if (value_start != std::string::npos) {
                    size_t value_end = value_start;
                    int brace_balance = 0;
                    for (; value_end < content.length(); ++value_end) {
                        char c = content[value_end];
                        if (c == '{') brace_balance++;
                        else if (c == '}') brace_balance--;
                        else if (c == ',' && brace_balance == 0) break;
                    }
                    std::string value = content.substr(value_start, value_end - value_start);
                    value.erase(value.find_last_not_of(" \t\n\r,") + 1);
                    result.append(value);
                }
            }
        } else {
            result.append(match.str(0));
        }
        last_pos = match.position() + match.length();
    }
    result.append(source, last_pos, std::string::npos);
    source = result;
}

void CHTLJSPreprocessor::process(std::string& source) {
    vir_map.clear();
    findDeclarations(source);
    substituteAccesses(source);
}
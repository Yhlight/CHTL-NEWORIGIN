#include "CodeMerger.h"
#include "CHTLCommon.h"
#include <sstream>
#include <regex>

namespace CHTL {

CodeMerger::CodeMerger()
    : outputType(OutputType::SEPARATE_FILES), inlineCSS(false), inlineJS(false), defaultStruct(false) {
    // Initialize default templates
    htmlTemplate = getDefaultHTMLTemplate();
    cssTemplate = getDefaultCSSTemplate();
    jsTemplate = getDefaultJSTemplate();
}

MergedOutput CodeMerger::merge(const std::vector<CodeFragment>& fragments) {
    MergedOutput output;
    
    // Separate fragments by type
    std::vector<CodeFragment> chtlFragments;
    std::vector<CodeFragment> chtljsFragments;
    std::vector<CodeFragment> cssFragments;
    std::vector<CodeFragment> jsFragments;
    
    for (const auto& fragment : fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL:
                chtlFragments.push_back(fragment);
                break;
            case FragmentType::CHTLJS:
                chtljsFragments.push_back(fragment);
                break;
            case FragmentType::CSS:
                cssFragments.push_back(fragment);
                break;
            case FragmentType::JS:
                jsFragments.push_back(fragment);
                break;
            default:
                // Unknown fragment type - skip
                break;
        }
    }
    
    return mergeFragments(chtlFragments, chtljsFragments, cssFragments, jsFragments);
}

MergedOutput CodeMerger::mergeFragments(const std::vector<CodeFragment>& chtlFragments,
                                       const std::vector<CodeFragment>& chtljsFragments,
                                       const std::vector<CodeFragment>& cssFragments,
                                       const std::vector<CodeFragment>& jsFragments) {
    MergedOutput output;
    
    // Generate HTML from CHTL fragments
    output.html = generateHTML(chtlFragments);
    
    // Generate CSS from CSS fragments
    output.css = generateCSS(cssFragments);
    
    // Generate JS from JS and CHTL JS fragments
    std::vector<CodeFragment> allJSFragments = jsFragments;
    allJSFragments.insert(allJSFragments.end(), chtljsFragments.begin(), chtljsFragments.end());
    output.js = generateJS(allJSFragments);
    
    // Apply inline options
    if (inlineCSS && !output.css.empty()) {
        output.html = inlineCSSIntoHTML(output.html, output.css);
        output.css.clear();
    }
    
    if (inlineJS && !output.js.empty()) {
        output.html = inlineJSIntoHTML(output.html, output.js);
        output.js.clear();
    }
    
    // Generate separate files if needed
    if (outputType == OutputType::SEPARATE_FILES) {
        if (!output.css.empty()) {
            output.separateFiles["style.css"] = output.css;
        }
        if (!output.js.empty()) {
            output.separateFiles["script.js"] = output.js;
        }
    }
    
    return output;
}

void CodeMerger::registerPlaceholder(const std::string& placeholder, const std::string& content) {
    placeholderMap[placeholder] = content;
    reversePlaceholderMap[content] = placeholder;
}

std::string CodeMerger::resolvePlaceholders(const std::string& content) {
    std::string result = content;
    
    // Replace placeholders with actual content
    for (const auto& pair : placeholderMap) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

void CodeMerger::clearPlaceholders() {
    placeholderMap.clear();
    reversePlaceholderMap.clear();
}

std::string CodeMerger::generateHTML(const std::vector<CodeFragment>& fragments) {
    std::stringstream html;
    
    if (defaultStruct) {
        html << "<!DOCTYPE html>\n";
        html << "<html lang=\"en\">\n";
        html << "<head>\n";
        html << "    <meta charset=\"UTF-8\">\n";
        html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        html << "    <title>CHTL Document</title>\n";
        if (!inlineCSS) {
            html << "    <link rel=\"stylesheet\" href=\"style.css\">\n";
        }
        html << "</head>\n";
        html << "<body>\n";
    }
    
    // Process CHTL fragments
    for (const auto& fragment : fragments) {
        std::string processed = processCHTLFragment(fragment);
        html << processed << "\n";
    }
    
    if (defaultStruct) {
        if (!inlineJS) {
            html << "    <script src=\"script.js\"></script>\n";
        }
        html << "</body>\n";
        html << "</html>\n";
    }
    
    return formatHTML(html.str());
}

std::string CodeMerger::generateCSS(const std::vector<CodeFragment>& fragments) {
    std::stringstream css;
    
    // Process CSS fragments
    for (const auto& fragment : fragments) {
        std::string processed = processCSSFragment(fragment);
        css << processed << "\n";
    }
    
    return formatCSS(css.str());
}

std::string CodeMerger::generateJS(const std::vector<CodeFragment>& fragments) {
    std::stringstream js;
    
    // Process JS and CHTL JS fragments
    for (const auto& fragment : fragments) {
        std::string processed;
        if (fragment.type == FragmentType::CHTLJS) {
            processed = processCHTLJSFragment(fragment);
        } else {
            processed = processJSFragment(fragment);
        }
        js << processed << "\n";
    }
    
    return formatJS(js.str());
}

std::string CodeMerger::getDefaultHTMLTemplate() const {
    return R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{TITLE}}</title>
    {{CSS_LINK}}
</head>
<body>
    {{CONTENT}}
    {{JS_SCRIPT}}
</body>
</html>)";
}

std::string CodeMerger::getDefaultCSSTemplate() const {
    return R"(/* CHTL Generated CSS */
{{CONTENT}})";
}

std::string CodeMerger::getDefaultJSTemplate() const {
    return R"(/* CHTL Generated JavaScript */
{{CONTENT}})";
}

std::string CodeMerger::mergeCHTLFragments(const std::vector<CodeFragment>& fragments) {
    std::stringstream result;
    
    for (const auto& fragment : fragments) {
        std::string processed = processCHTLFragment(fragment);
        result << processed << "\n";
    }
    
    return result.str();
}

std::string CodeMerger::mergeCHTLJSFragments(const std::vector<CodeFragment>& fragments) {
    std::stringstream result;
    
    for (const auto& fragment : fragments) {
        std::string processed = processCHTLJSFragment(fragment);
        result << processed << "\n";
    }
    
    return result.str();
}

std::string CodeMerger::mergeCSSFragments(const std::vector<CodeFragment>& fragments) {
    std::stringstream result;
    
    for (const auto& fragment : fragments) {
        std::string processed = processCSSFragment(fragment);
        result << processed << "\n";
    }
    
    return result.str();
}

std::string CodeMerger::mergeJSFragments(const std::vector<CodeFragment>& fragments) {
    std::stringstream result;
    
    for (const auto& fragment : fragments) {
        std::string processed = processJSFragment(fragment);
        result << processed << "\n";
    }
    
    return result.str();
}

std::string CodeMerger::processFragment(const CodeFragment& fragment) {
    switch (fragment.type) {
        case FragmentType::CHTL:
            return processCHTLFragment(fragment);
        case FragmentType::CHTLJS:
            return processCHTLJSFragment(fragment);
        case FragmentType::CSS:
            return processCSSFragment(fragment);
        case FragmentType::JS:
            return processJSFragment(fragment);
        default:
            return fragment.content;
    }
}

std::string CodeMerger::processCHTLFragment(const CodeFragment& fragment) {
    // TODO: Implement CHTL fragment processing
    // This would involve parsing CHTL syntax and converting to HTML
    
    // For now, return the content as-is
    return fragment.content;
}

std::string CodeMerger::processCHTLJSFragment(const CodeFragment& fragment) {
    // TODO: Implement CHTL JS fragment processing
    // This would involve parsing CHTL JS syntax and converting to JavaScript
    
    // For now, return the content as-is
    return fragment.content;
}

std::string CodeMerger::processCSSFragment(const CodeFragment& fragment) {
    // TODO: Implement CSS fragment processing
    // This would involve parsing CSS syntax and applying CHTL enhancements
    
    // For now, return the content as-is
    return fragment.content;
}

std::string CodeMerger::processJSFragment(const CodeFragment& fragment) {
    // TODO: Implement JS fragment processing
    // This would involve parsing JavaScript syntax
    
    // For now, return the content as-is
    return fragment.content;
}

std::string CodeMerger::replacePlaceholders(const std::string& content) {
    return resolvePlaceholders(content);
}

std::string CodeMerger::restorePlaceholders(const std::string& content) {
    std::string result = content;
    
    // Restore placeholders from content
    for (const auto& pair : reversePlaceholderMap) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

std::string CodeMerger::formatHTML(const std::string& content) {
    // TODO: Implement HTML formatting
    return content;
}

std::string CodeMerger::formatCSS(const std::string& content) {
    // TODO: Implement CSS formatting
    return content;
}

std::string CodeMerger::formatJS(const std::string& content) {
    // TODO: Implement JavaScript formatting
    return content;
}

std::string CodeMerger::processTemplate(const std::string& template_, const std::map<std::string, std::string>& variables) {
    std::string result = template_;
    
    for (const auto& pair : variables) {
        std::string placeholder = "{{" + pair.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

std::map<std::string, std::string> CodeMerger::extractTemplateVariables(const std::string& template_) {
    std::map<std::string, std::string> variables;
    
    std::regex placeholderRegex(R"(\{\{(\w+)\}\})");
    std::sregex_iterator iter(template_.begin(), template_.end(), placeholderRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string variable = match[1].str();
        variables[variable] = "";
    }
    
    return variables;
}

std::string CodeMerger::inlineCSSIntoHTML(const std::string& html, const std::string& css) {
    std::string result = html;
    
    // Find head tag and insert style
    size_t headPos = result.find("<head>");
    if (headPos != std::string::npos) {
        headPos += 6; // Length of "<head>"
        std::string styleTag = "\n    <style>\n" + css + "\n    </style>\n";
        result.insert(headPos, styleTag);
    }
    
    return result;
}

std::string CodeMerger::inlineJSIntoHTML(const std::string& html, const std::string& js) {
    std::string result = html;
    
    // Find body end tag and insert script
    size_t bodyEndPos = result.find("</body>");
    if (bodyEndPos != std::string::npos) {
        std::string scriptTag = "\n    <script>\n" + js + "\n    </script>\n";
        result.insert(bodyEndPos, scriptTag);
    }
    
    return result;
}

} // namespace CHTL
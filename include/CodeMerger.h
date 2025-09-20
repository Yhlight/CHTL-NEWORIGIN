#pragma once

#include "CHTLCommon.h"
#include "UnifiedScanner.h"
#include <memory>

namespace CHTL {

// Output types
enum class OutputType {
    HTML,
    CSS,
    JS,
    SEPARATE_FILES
};

// Merged output structure
struct MergedOutput {
    std::string html;
    std::string css;
    std::string js;
    std::map<std::string, std::string> separateFiles;
    
    bool isEmpty() const {
        return html.empty() && css.empty() && js.empty() && separateFiles.empty();
    }
};

// Code merger - combines different code fragments into final output
class CodeMerger {
private:
    OutputType outputType;
    bool inlineCSS;
    bool inlineJS;
    bool defaultStruct;
    
    // Placeholder resolution
    std::map<std::string, std::string> placeholderMap;
    std::map<std::string, std::string> reversePlaceholderMap;
    
    // Output templates
    std::string htmlTemplate;
    std::string cssTemplate;
    std::string jsTemplate;
    
public:
    CodeMerger();
    ~CodeMerger() = default;
    
    // Configuration
    void setOutputType(OutputType type) { outputType = type; }
    void setInlineCSS(bool inline_) { inlineCSS = inline_; }
    void setInlineJS(bool inline_) { inlineJS = inline_; }
    void setDefaultStruct(bool default_) { defaultStruct = default_; }
    
    // Main merging methods
    MergedOutput merge(const std::vector<CodeFragment>& fragments);
    MergedOutput mergeFragments(const std::vector<CodeFragment>& chtlFragments,
                               const std::vector<CodeFragment>& chtljsFragments,
                               const std::vector<CodeFragment>& cssFragments,
                               const std::vector<CodeFragment>& jsFragments);
    
    // Placeholder management
    void registerPlaceholder(const std::string& placeholder, const std::string& content);
    std::string resolvePlaceholders(const std::string& content);
    void clearPlaceholders();
    
    // Output generation
    std::string generateHTML(const std::vector<CodeFragment>& fragments);
    std::string generateCSS(const std::vector<CodeFragment>& fragments);
    std::string generateJS(const std::vector<CodeFragment>& fragments);
    
    // Template management
    void setHTMLTemplate(const std::string& template_) { htmlTemplate = template_; }
    void setCSSTemplate(const std::string& template_) { cssTemplate = template_; }
    void setJSTemplate(const std::string& template_) { jsTemplate = template_; }
    
    // Utility methods
    std::string getDefaultHTMLTemplate() const;
    std::string getDefaultCSSTemplate() const;
    std::string getDefaultJSTemplate() const;
    
private:
    // Internal merging helpers
    std::string mergeCHTLFragments(const std::vector<CodeFragment>& fragments);
    std::string mergeCHTLJSFragments(const std::vector<CodeFragment>& fragments);
    std::string mergeCSSFragments(const std::vector<CodeFragment>& fragments);
    std::string mergeJSFragments(const std::vector<CodeFragment>& fragments);
    
    // Fragment processing
    std::string processFragment(const CodeFragment& fragment);
    std::string processCHTLFragment(const CodeFragment& fragment);
    std::string processCHTLJSFragment(const CodeFragment& fragment);
    std::string processCSSFragment(const CodeFragment& fragment);
    std::string processJSFragment(const CodeFragment& fragment);
    
    // Placeholder resolution
    std::string replacePlaceholders(const std::string& content);
    std::string restorePlaceholders(const std::string& content);
    
    // Output formatting
    std::string formatHTML(const std::string& content);
    std::string formatCSS(const std::string& content);
    std::string formatJS(const std::string& content);
    
    // Template processing
    std::string processTemplate(const std::string& template_, const std::map<std::string, std::string>& variables);
    std::map<std::string, std::string> extractTemplateVariables(const std::string& template_);
};

} // namespace CHTL
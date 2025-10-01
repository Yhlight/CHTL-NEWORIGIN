#pragma once

#include <memory>
#include <string>

namespace CHTL {

// Forward declarations
class CHTLContext;
class CHTLNode;

// Generator class for HTML/CSS/JS output
class CHTLGenerator {
public:
    CHTLGenerator(CHTLContext& context);
    ~CHTLGenerator();
    
    // Main generation methods
    bool generate(const CHTLNode& ast);
    
    // Output methods
    bool writeOutput(const std::string& htmlContent, 
                    const std::string& cssContent, 
                    const std::string& jsContent);
    
private:
    // Node-specific generation methods
    bool generateNode(const CHTLNode& node);
    bool generateElement(const CHTLNode& node);
    bool generateText(const CHTLNode& node);
    bool generateStyle(const CHTLNode& node);
    bool generateScript(const CHTLNode& node);
    bool generateTemplate(const CHTLNode& node);
    bool generateCustom(const CHTLNode& node);
    bool generateImport(const CHTLNode& node);
    
    CHTLContext& context_;
};

} // namespace CHTL
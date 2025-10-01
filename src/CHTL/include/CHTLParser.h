#pragma once

#include <memory>
#include <string>
#include <vector>

namespace CHTL {

// Forward declarations
class CHTLContext;
class CHTLNode;
struct Token;

// Parser class - placeholder for ANTLR4 integration
class CHTLParser {
public:
    CHTLParser(CHTLContext& context);
    ~CHTLParser();
    
    // Parsing methods
    std::unique_ptr<CHTLNode> parse(const std::vector<Token>& tokens);
    bool parseFile(const std::string& filepath);
    std::unique_ptr<CHTLNode> getAST() const;
    
private:
    CHTLContext& context_;
    std::unique_ptr<CHTLNode> ast_;
};

} // namespace CHTL
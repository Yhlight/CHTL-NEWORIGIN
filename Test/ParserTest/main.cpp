#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLTransformer/AstTransformer.h"
#include "../../CHTL/CHTLGenerator/HtmlGenerator.h"
#include <iostream>
#include <cassert>
#include <regex>

int main() {
    std::string source = R"(
[Template] @Style DefaultBox
{
    width: 100px;
    height: 100px;
}

div {
    id: "my-box";
    style {
        @Style DefaultBox;
        background-color: blue;
    }
}
)";

    // 1. Lexing
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.getAllTokens();

    // 2. Parsing
    Parser parser(tokens);
    std::unique_ptr<RootNode> rawAst = parser.parse();
    assert(rawAst != nullptr);
    std::cout << "Parser produced a raw AST." << std::endl;

    // 3. Transformation
    AstTransformer transformer;
    std::unique_ptr<RootNode> expandedAst = transformer.transform(std::move(rawAst));
    assert(expandedAst != nullptr);
    std::cout << "Transformer produced an expanded AST." << std::endl;

    // 4. Code Generation
    HtmlGenerator generator;
    std::string html = generator.generate(*expandedAst);

    std::cout << "\n--- Generated HTML ---" << std::endl;
    std::cout << html << std::endl;
    std::cout << "----------------------" << std::endl;

    // 5. Final Verification
    std::string expectedHtml = R"(<div id="my-box" style="width:100px;height:100px;background-color:blue;">
</div>
)";

    auto normalize = [](const std::string& s) {
        std::string result = std::regex_replace(s, std::regex("\\s+"), " ");
        result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");
        return result;
    };

    assert(normalize(html) == normalize(expectedHtml));

    std::cout << "Template expansion test passed!" << std::endl;

    return 0;
}

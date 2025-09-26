#include "CompilerDispatcher.h"
#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <string>
#include <vector>
#include <memory>

std::string CompilerDispatcher::compile(const std::string& source) {
    std::vector<CodeFragment> fragments = scanner.scan(source);
    std::vector<std::unique_ptr<BaseNode>> finalAst;
    std::string globalCss;
    SharedContext sharedContext;
    bool outputDoctype = false;

    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            if (fragment.content.empty()) continue;
            Lexer lexer(fragment.content);
            Parser parser(lexer);
            auto ast = parser.parse();
            for (auto& node : ast) {
                finalAst.push_back(std::move(node));
            }
            globalCss += parser.globalStyleContent;
            if (parser.outputHtml5Doctype) {
                outputDoctype = true;
            }
        } else if (fragment.type == FragmentType::CSS) {
            globalCss += fragment.content;
        } else if (fragment.type == FragmentType::JS) {
            // JS fragments are ignored until a JS compiler/handler is integrated.
            continue;
        } else if (fragment.type == FragmentType::CHTL_JS) {
            // CHTL_JS fragments are ignored until the CHTL JS compiler is integrated.
            continue;
        }
    }

    Generator generator;
    std::string htmlOutput = generator.generate(finalAst, globalCss, sharedContext, outputDoctype);

    return htmlOutput;
}
#include "CompilerDispatcher.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLContext/Context.h"
#include "CHTL/CHTLResolver/Resolver.h"
#include <stdexcept>

namespace CHTL {

CompilationResult CompilerDispatcher::dispatch(const std::string& chtl_source) {

    // 0. Setup
    Context context;

    // 1. Lex
    Lexer lexer(chtl_source);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parse (populates the context with declarations)
    Parser parser(tokens, context);
    std::unique_ptr<ElementNode> ast = parser.parse();

    if (!ast) {
        // It's possible to have a file with only templates, so no root element.
        // In that case, we just return an empty result.
        return CompilationResult();
    }

    // 3. Resolve (transforms the AST by applying templates)
    Resolver resolver(context);
    resolver.resolve(*ast);

    // 4. Generate
    Generator generator;
    CompilationResult result = generator.generate(*ast);

    return result;
}

} // namespace CHTL

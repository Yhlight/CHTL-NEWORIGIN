#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>

#include "CHTLContext/Context.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ConfigurationNode.h"
#include "CHTLNode/ConfigPropertyNode.h"

// A simple visitor to find and apply configuration
class Configurator : public NodeVisitor {
public:
    explicit Configurator(CHTLContext& context) : context(context) {}

    void apply(const std::vector<std::shared_ptr<BaseNode>>& nodes) {
        for (const auto& node : nodes) {
            if (auto config_node = std::dynamic_pointer_cast<ConfigurationNode>(node)) {
                config_node->accept(*this);
            }
        }
    }

    void visit(ConfigurationNode& node) override {
        for (const auto& child : node.children) {
            child->accept(*this);
        }
    }

    void visit(ConfigPropertyNode& node) override {
        if (node.key == "DEBUG_MODE") {
            context.config.debug_mode = (node.value == "true");
        }
    }

    void visit(ElementNode& node) override {}
    void visit(TextNode& node) override {}
    void visit(StyleBlockNode& node) override {}
    void visit(StylePropertyNode& node) override {}
    void visit(TemplateDefinitionNode& node) override {}
    void visit(TemplateUsageNode& node) override {}

private:
    CHTLContext& context; // The missing member variable
};


int main(int argc, char* argv[]) {
    std::ifstream file("test.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open test.chtl" << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    CHTLContext context;
    Lexer default_lexer(source, context);
    std::vector<Token> initial_tokens = default_lexer.scan_tokens();
    Parser config_parser(initial_tokens);
    try {
        auto ast = config_parser.parse();
        Configurator configurator(context);
        configurator.apply(ast);
    } catch (const ParseError& e) {
        // Ignore
    }

    std::cout << "--- Compiler Run ---" << std::endl;
    std::cout << "Debug Mode: " << (context.config.debug_mode ? "Enabled" : "Disabled") << std::endl;

    Lexer main_lexer(source, context);
    std::vector<Token> main_tokens = main_lexer.scan_tokens();
    Parser main_parser(main_tokens);
    try {
        auto ast = main_parser.parse();
        Generator generator;
        std::string html_output = generator.generate(ast);

        std::cout << "\n--- HTML Output ---" << std::endl;
        std::cout << html_output;
        std::cout << "-------------------" << std::endl;

    } catch (const ParseError& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#include "catch.hpp"
#include "../src/CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "../src/CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "../src/CHTL_JS/CHTLJSNode/ScriptLoaderNode.h"
#include "../src/CHTL_JS/CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../src/CHTL_JS/CHTLJSNode/ListenNode.h"
#include "../src/CHTL_JS/CHTLJSNode/EventBindingNode.h"
#include "../src/CHTL_JS/CHTLJSNode/DelegateNode.h"
#include "../src/CHTL_JS/CHTLJSNode/AnimateNode.h"
#include "../src/CHTL_JS/CHTLJSNode/VirtualObjectNode.h"
#include "../src/CHTL_JS/CHTLJSNode/RouterNode.h"
#include <vector>
#include <string>
#include <memory>

TEST_CASE("CHTLJS Parser and Generator for ScriptLoader", "[CHTLJS]") {
    // 1. Define the CHTL JS source code for the ScriptLoader
    std::string source = R"(
        ScriptLoader {
            load: "./module1.js",
            load: "./module2.cjjs",
        }
    )";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    // Ensure the parsed node is of the correct type
    auto* script_loader_node = dynamic_cast<ScriptLoaderNode*>(ast.get());
    REQUIRE(script_loader_node != nullptr);

    // Check the content of the parsed node
    const auto& files = script_loader_node->getFiles();
    REQUIRE(files.size() == 2);
    REQUIRE(files[0] == "./module1.js");
    REQUIRE(files[1] == "./module2.cjjs");

    // 3. Generate the JavaScript code from the AST
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    std::string expected_js =
        "Louder.require(['./module1.js', './module2.cjjs'], function() {\n"
        "    // All scripts have been loaded\n"
        "});\n";

    REQUIRE(generated_js == expected_js);
}

TEST_CASE("CHTLJS Parser and Generator for Router block", "[CHTLJS]") {
    // 1. Define the CHTL JS source code
    std::string source = R"(Router {
        url: "/home",
        page: {{'#home-page'}}
    })";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    auto* router_node = dynamic_cast<RouterNode*>(ast.get());
    REQUIRE(router_node != nullptr);

    // Check the content of the parsed node
    REQUIRE(router_node->getRoutes().size() == 1);
    REQUIRE(router_node->getRoutes().at("/home") == "{{'#home-page'}}");

    // 3. Generate the JavaScript code
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    std::string expected_js = "Router.add('/home', document.querySelector('#home-page'));\n";

    REQUIRE(generated_js == expected_js);
}

TEST_CASE("CHTLJS Parser and Generator for Vir object declaration", "[CHTLJS]") {
    // 1. Define the CHTL JS source code
    std::string source = R"(Vir test = Listen {
        click: () => {}
    };)";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    auto* vir_node = dynamic_cast<VirtualObjectNode*>(ast.get());
    REQUIRE(vir_node != nullptr);

    // Check the content of the parsed node
    REQUIRE(vir_node->getName() == "test");
    REQUIRE(vir_node->getAssignedBlock() != nullptr);

    // 3. Generate the JavaScript code
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    // The 'Vir' keyword is compile-time only. The output should be a standard variable assignment.
    std::string expected_js = "const test = {\n    click: () => {}\n};\n";

    REQUIRE(generated_js == expected_js);
}

TEST_CASE("CHTLJS Parser and Generator for Animate block", "[CHTLJS]") {
    // 1. Define the CHTL JS source code
    std::string source = R"(const anim = Animate {
        target: {{'#box'}},
        duration: 1000,
        begin: { opacity: 0 },
        end: { opacity: 1 }
    })";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    auto* animate_node = dynamic_cast<AnimateNode*>(ast.get());
    REQUIRE(animate_node != nullptr);

    // Check the content of the parsed node
    REQUIRE(animate_node->getProperties().at("target") == "{{'#box'}}");
    REQUIRE(animate_node->getProperties().at("duration") == "1000");
    REQUIRE(animate_node->getProperties().at("begin") == "{ opacity: 0 }");
    REQUIRE(animate_node->getProperties().at("end") == "{ opacity: 1 }");

    // 3. Generate the JavaScript code
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    // Note: std::map sorts keys alphabetically, so the order in the generated object is predictable.
    std::string expected_js =
        "new Animate({\n"
        "begin: { opacity: 0 },\n"
        "duration: 1000,\n"
        "end: { opacity: 1 },\n"
        "target: document.querySelector('#box'),\n"
        "});\n";

    REQUIRE(generated_js == expected_js);
}

TEST_CASE("CHTLJS Parser and Generator for Delegate block", "[CHTLJS]") {
    // 1. Define the CHTL JS source code
    std::string source = R"({{'.container'}}->Delegate {
        target: {{'.btn'}},
        click: (event) => { console.log('Delegated click!'); }
    })";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    auto* delegate_node = dynamic_cast<DelegateNode*>(ast.get());
    REQUIRE(delegate_node != nullptr);

    // Check the content of the parsed node
    REQUIRE(delegate_node->getParentSelector() == "'.container'");
    REQUIRE(delegate_node->getTargetSelector() == "'.btn'");
    REQUIRE(delegate_node->getEvents().size() == 1);
    REQUIRE(delegate_node->getEvents().at("click") == "(event) => { console.log('Delegated click!'); }");

    // 3. Generate the JavaScript code
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    std::string expected_js =
        "document.querySelector('.container').addEventListener('click', (event) => {\n"
        "    if (event.target.matches('.btn')) {\n"
        "        ((event) => { console.log('Delegated click!'); }).call(event.target, event);\n"
        "    }\n"
        "});\n";

    REQUIRE(generated_js == expected_js);
}

TEST_CASE("CHTLJS Parser and Generator for Event Binding Operator", "[CHTLJS]") {
    // 1. Define the CHTL JS source code
    std::string source = "{{'#my-btn'}} &-> click: () => { console.log('clicked'); };";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    auto* binding_node = dynamic_cast<EventBindingNode*>(ast.get());
    REQUIRE(binding_node != nullptr);

    // Check the content of the parsed node
    REQUIRE(binding_node->getSelector() == "'#my-btn'");
    REQUIRE(binding_node->getEventName() == "click");
    REQUIRE(binding_node->getCallback() == "() => { console.log('clicked'); }");

    // 3. Generate the JavaScript code
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    std::string expected_js = "document.querySelector('#my-btn').addEventListener('click', () => { console.log('clicked'); });\n";

    REQUIRE(generated_js == expected_js);
}

TEST_CASE("CHTLJS Parser and Generator for Listen block", "[CHTLJS]") {
    // 1. Define the CHTL JS source code
    std::string source = R"({{'.btn'}}->Listen {
        click: () => { alert('Button clicked!'); },
        mouseover: function() { this.style.color = 'red'; }
    })";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    auto* listen_node = dynamic_cast<ListenNode*>(ast.get());
    REQUIRE(listen_node != nullptr);

    // Check the content of the parsed node
    REQUIRE(listen_node->getSelector() == "'.btn'");
    REQUIRE(listen_node->getEvents().size() == 2);
    REQUIRE(listen_node->getEvents().at("click") == "() => { alert('Button clicked!'); }");
    REQUIRE(listen_node->getEvents().at("mouseover") == "function() { this.style.color = 'red'; }");

    // 3. Generate the JavaScript code
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    std::string expected_js =
        "document.querySelector('.btn').addEventListener('click', () => { alert('Button clicked!'); });\n"
        "document.querySelector('.btn').addEventListener('mouseover', function() { this.style.color = 'red'; });\n";

    REQUIRE(generated_js == expected_js);
}

TEST_CASE("CHTLJS Parser and Generator for Enhanced Selector", "[CHTLJS]") {
    // 1. Define the CHTL JS source code
    std::string source = "{{.my-class}}->someFunction();";

    // 2. Parse the source code
    CHTLJSParser parser;
    std::unique_ptr<CHTLJSBaseNode> ast = parser.parse(source);

    // Ensure the parser returned a valid AST
    REQUIRE(ast != nullptr);
    auto* selector_node = dynamic_cast<EnhancedSelectorNode*>(ast.get());
    REQUIRE(selector_node != nullptr);

    // Check the content of the parsed node
    REQUIRE(selector_node->getSelector() == ".my-class");
    REQUIRE(selector_node->getExpression() == "someFunction()");

    // 3. Generate the JavaScript code
    CHTLJSGenerator generator;
    std::string generated_js = generator.generate(*ast);

    // 4. Assert the generated code is correct
    std::string expected_js = "document.querySelector('.my-class').someFunction();\n";

    REQUIRE(generated_js == expected_js);
}
#include "../catch.hpp"
#include "../src/CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "../src/CHTL_JS/CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "../src/CHTL_JS/CHTLJSNode/ListenNode.h"
#include "../src/CHTL_JS/CHTLJSNode/EventBindingNode.h"
#include "../src/CHTL_JS/CHTLJSNode/AnimateNode.h"
#include "../src/CHTL_JS/CHTLJSNode/RouterNode.h"
#include "../src/CHTL_JS/CHTLJSNode/DelegateNode.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLNode/ElementNode.h"
#include "../src/CHTL/CHTLNode/ScriptNode.h"
#include <string>

// Helper to remove whitespace for consistent test comparisons
std::string remove_whitespace(const std::string& str) {
    std::string out;
    std::remove_copy_if(str.begin(), str.end(), std::back_inserter(out), ::isspace);
    return out;
}

TEST_CASE("CHTLJS Generator: Enhanced Selector", "[chtljs_generator]") {
    CHTLJSGenerator generator;

    SECTION("Single element selector (ID)") {
        EnhancedSelectorNode node("#my-id", "textContent = 'hello'");
        std::string expected = "document.querySelector('#my-id').textContent = 'hello';";
        REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
    }

    SECTION("Multiple element selector (class)") {
        EnhancedSelectorNode node(".my-class", "");
        std::string expected = "document.querySelectorAll('.my-class');";
        REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
    }

    SECTION("Indexed element selector") {
        EnhancedSelectorNode node("div[2]", "style.color = 'red'");
        std::string expected = "document.querySelectorAll('div')[2].style.color = 'red';";
        REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
    }
}

TEST_CASE("CHTLJS Generator: Listen Block", "[chtljs_generator]") {
    CHTLJSGenerator generator;

    SECTION("Listen on a single element") {
        ListenNode node("{{#my-button}}", {{"click", "() => console.log('clicked')"}});
        std::string expected = "document.querySelector('#my-button').addEventListener('click', () => console.log('clicked'));";
        REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
    }

    SECTION("Listen on multiple elements") {
        ListenNode node("{{.card}}", {{"mouseenter", "onEnter"}, {"mouseleave", "onLeave"}});
        std::string expected = R"(
            document.querySelectorAll('.card').forEach(element => {
                element.addEventListener('mouseenter', onEnter);
                element.addEventListener('mouseleave', onLeave);
            });
        )";
        REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
    }
}

TEST_CASE("CHTLJS Generator: Event Binding Operator", "[chtljs_generator]") {
    CHTLJSGenerator generator;
    EventBindingNode node("{{.item[0]}}", "focus", "handleFocus");
    std::string expected = "document.querySelectorAll('.item')[0].addEventListener('focus', handleFocus);";
    REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
}

TEST_CASE("CHTLJS Generator: Animate", "[chtljs_generator]") {
    CHTLJSGenerator generator;
    std::map<std::string, std::string> props = {
        {"target", "{{#box}}"},
        {"duration", "1000"},
        {"easing", "'ease-in-out'"}
    };
    AnimateNode node(props);
    std::string expected = R"(
        new Animate({
            target: document.querySelector('#box'),
            duration: 1000,
            easing: 'ease-in-out',
        });
    )";
    REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
}

TEST_CASE("CHTLJS Generator: Router", "[chtljs_generator]") {
    CHTLJSGenerator generator;
    std::map<std::string, std::string> routes = {
        {"/home", "{{#home-page}}"},
        {"/about", "{{.about-page}}"}
    };
    RouterNode node(routes);
    std::string expected = R"(
        Router.add('/home', document.querySelector('#home-page'));
        Router.add('/about', document.querySelectorAll('.about-page'));
    )";
    REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
}

TEST_CASE("CHTLJS Generator: Delegate", "[chtljs_generator]") {
    CHTLJSGenerator generator;
    DelegateNode node("{{#app}}", "{{.list-item}}", {{"click", "onItemClick"}});
    std::string expected = R"(
        document.querySelector('#app').addEventListener('click', (event) => {
            if (event.target.matches('.list-item')) {
                (onItemClick).call(event.target, event);
            }
        });
    )";
    REQUIRE(remove_whitespace(generator.generate(node)) == remove_whitespace(expected));
}

TEST_CASE("CHTL Parser: Automatic Attribute Injection", "[chtl_parser]") {
    SECTION("Auto-inject class") {
        std::string source = R"(
            div {
                script {
                    const btn = {{.my-button}};
                }
            }
        )";
        Lexer lexer(source);
        Parser parser(lexer);
        auto nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto* element = static_cast<ElementNode*>(nodes[0].get());
        REQUIRE(element->attributes.count("class") == 1);
        REQUIRE(element->attributes.at("class")->toString() == "my-button");
    }

    SECTION("Auto-inject ID") {
        std::string source = R"(
            div {
                script {
                    const main = {{#app-root}};
                }
            }
        )";
        Lexer lexer(source);
        Parser parser(lexer);
        auto nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto* element = static_cast<ElementNode*>(nodes[0].get());
        REQUIRE(element->attributes.count("id") == 1);
        REQUIRE(element->attributes.at("id")->toString() == "app-root");
    }

    SECTION("Do not inject if attribute exists") {
        std::string source = R"(
            div {
                id: "manual-id";
                script {
                    const main = {{#app-root}};
                }
            }
        )";
        Lexer lexer(source);
        Parser parser(lexer);
        auto nodes = parser.parse();
        REQUIRE(nodes.size() == 1);
        auto* element = static_cast<ElementNode*>(nodes[0].get());
        REQUIRE(element->attributes.count("id") == 1);
        REQUIRE(element->attributes.at("id")->toString() == "manual-id");
    }
}
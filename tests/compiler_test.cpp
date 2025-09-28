#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../src/CHTLJS/CHTLJSCompiler.h"
#include <string>
#include <algorithm>

// Helper to remove whitespace for easier comparison
std::string remove_whitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}

TEST_CASE("CHTLJSCompiler tests", "[compiler]") {
    CHTLJSCompiler compiler;

    SECTION("Enhanced Animate function compilation") {
        std::string source = R"RAW(
            Animate {
                target: {{.my-div}},
                duration: 1000,
                easing: 'ease-out',
                loop: -1,
                when: [ {at: 0.5, transform: 'scale(1.1)'} ],
                end: { opacity: '0' }
            };
        )RAW";
        std::string result = compiler.compile(source);
        std::string expected = R"JS(
            const target_elem = document.querySelector('.my-div');
            if (target_elem) {
                const keyframes = [ { offset: 0.5, transform: 'scale(1.1)' }, { opacity: '0' } ];
                const animation = target_elem.animate(keyframes, { duration: 1000, easing: 'ease-out', iterations: Infinity });
            }
        )JS";
        REQUIRE(remove_whitespace(result) == remove_whitespace(expected));
    }

    SECTION("Delegate function compilation") {
        std::string source = R"RAW(
            {{#app}}->Delegate {
                target: '.list-item',
                click: itemClickHandler
            };
        )RAW";
        std::string result = compiler.compile(source);
        std::string expected = R"JS(
            const parent_elem = document.querySelector('#app');
            if (parent_elem) {
              parent_elem.addEventListener('click', (event) => {
                if (event.target.matches('.list-item')) {
                  itemClickHandler(event);
                }
              });
            }
        )JS";
        REQUIRE(remove_whitespace(result) == remove_whitespace(expected));
    }

    SECTION("Router function compilation") {
        std::string source = R"RAW(
            Router {
                mode: 'hash',
                url: '/', page: {{#home}},
                url: '/about', page: {{#about}}
            };
        )RAW";
        std::string result = compiler.compile(source);
        std::string expected = R"JS(
            const routes = {
                '/': document.querySelector('#home'),
                '/about': document.querySelector('#about'),
            };
            function navigate() {
                const path = window.location.hash.slice(1) || '/';
                const page = routes[path];
                // Hide all pages
                for (const key in routes) {
                    if (routes[key]) routes[key].style.display = 'none';
                }
                // Show the current page
                if (page) {
                    page.style.display = 'block';
                }
            }
            window.addEventListener('hashchange', navigate);
            navigate(); // Initial navigation
        )JS";
        REQUIRE(remove_whitespace(result) == remove_whitespace(expected));
    }

    SECTION("ScriptLoader function compilation") {
        std::string source = R"RAW(
            ScriptLoader {
                load: './a.js', ./b.js;
            };
        )RAW";
        std::string result = compiler.compile(source);
        std::string expected = R"JS(
            (function() {
              const script0 = document.createElement('script');
              script0.src = './a.js';
              document.head.appendChild(script0);
              const script1 = document.createElement('script');
              script1.src = './b.js';
              document.head.appendChild(script1);
            })();
        )JS";
        REQUIRE(remove_whitespace(result) == remove_whitespace(expected));
    }
}
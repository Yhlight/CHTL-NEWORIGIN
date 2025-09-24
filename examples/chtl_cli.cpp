#include <iostream>
#include <sstream>
#include <string>
#include "CompilerDispatcher/Dispatcher.h"
#include "CodeMerger/CodeMerger.h"

int main() {
    std::ostringstream ss;
    ss << std::cin.rdbuf();
    const std::string input = ss.str();

    dispatcher::CompilerDispatcher disp;
    auto res = disp.dispatch(input);

    codemerger::CodeMerger merger;
    codemerger::MergeInput mi{res.chtl, res.chtljs, res.cssPlaceholders, res.jsPlaceholders};
    auto out = merger.merge(mi);

    std::cout << "HTML:\n" << out.html << "\nCSS:\n" << out.css << "\nJS:\n" << out.js << "\n";
    return 0;
}


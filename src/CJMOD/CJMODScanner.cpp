#include "CJMODScanner.h"

// Mock implementation that simulates finding '3 ** 4'
std::unique_ptr<Arg> CJMODScanner::scan(const Arg& syntax_arg, const std::string& keyword) {
    // This mock simulates finding the expression '3 ** 4' when the keyword is '**'.
    // A real implementation would need a sophisticated scanning algorithm.
    if (keyword == "**") {
        auto result_arg = std::make_unique<Arg>();

        auto arg1 = std::make_unique<AtomArg>("$");
        arg1->fillValue("3");
        result_arg->arguments.push_back(std::move(arg1));

        auto arg2 = std::make_unique<AtomArg>("**");
        arg2->fillValue("**");
        result_arg->arguments.push_back(std::move(arg2));

        auto arg3 = std::make_unique<AtomArg>("$");
        arg3->fillValue("4");
        result_arg->arguments.push_back(std::move(arg3));

        return result_arg;
    }

    // Return an empty Arg for any other keyword
    return std::make_unique<Arg>();
}
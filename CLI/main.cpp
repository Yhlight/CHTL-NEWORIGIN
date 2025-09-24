#include "CLIApp.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        CHTL::CLI::CLIApp app;
        return app.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
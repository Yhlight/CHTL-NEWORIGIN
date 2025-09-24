#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler Initializing..." << std::endl;
    if (argc > 1) {
        std::cout << "Processing file: " << argv[1] << std::endl;
    } else {
        std::cout << "Usage: CHTL_Final <filename>" << std::endl;
    }
    return 0;
}

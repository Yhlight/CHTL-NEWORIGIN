#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib> // For system()
#include <fstream>
#include <sstream>
#include <cstdio> // For remove()
#include <algorithm>

// Helper to run a command and check for success
void run_command(const std::string& command) {
    int result = system(command.c_str());
    assert(result == 0);
}

// Helper to read a file's content
std::string read_file_content(const std::string& path) {
    std::ifstream file(path);
    assert(file.good());
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper to write a file
void write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    file.close();
}

// Helper to clean up generated files
void cleanup_files(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        remove(file.c_str());
    }
}

void run_cli_test(void (*test_func)(), const std::string& test_name) {
    try {
        test_func();
        std::cout << "[PASS] " << test_name << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "[FAIL] " << test_name << " - " << e.what() << std::endl;
    } catch (...) {
        std::cout << "[FAIL] " << test_name << " - Unknown exception" << std::endl;
    }
}

// --- Test Cases ---

void test_default_struct_flag() {
    const std::string test_file = "test_default.chtl";
    const std::string html_file = "test_default.html";
    write_file(test_file, "p { text: \"Hello\"; }");

    run_command("./chtl_compiler " + test_file + " --default-struct");

    std::string content = read_file_content(html_file);
    assert(content.find("<!DOCTYPE html>") != std::string::npos);
    assert(content.find("<html>") != std::string::npos);
    assert(content.find("<head>") != std::string::npos);
    assert(content.find("<body>") != std::string::npos);
    assert(content.find("<p>Hello</p>") != std::string::npos);

    cleanup_files({test_file, html_file});
}


int main() {
    std::cout << "Running CLI tests..." << std::endl;
    run_cli_test(test_default_struct_flag, "Default Structure Flag (--default-struct)");
    std::cout << "\nCLI tests finished." << std::endl;
    return 0;
}
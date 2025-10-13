#include "chtl/Compiler.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "用法: chtlc <input.chtl> [output.html]\n";
        return 1;
    }
    const char* inPath = argv[1];
    const char* outPath = argc > 2 ? argv[2] : "out.html";

    std::ifstream ifs(inPath, std::ios::binary);
    if (!ifs) {
        std::cerr << "无法读取文件: " << inPath << "\n";
        return 2;
    }
    std::string source((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    try {
        auto res = chtl::Compiler::compile(source);
        std::ofstream ofs(outPath, std::ios::binary);
        ofs << res.html;
        std::cout << "生成: " << outPath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "编译失败: " << e.what() << "\n";
        return 3;
    }

    return 0;
}

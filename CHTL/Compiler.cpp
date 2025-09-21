#include "Compiler.h"
#include "CMODSystem/CMODSystem.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace CHTL {

Compiler::Compiler() {}

void Compiler::compile(const std::string& entryFilePath) {
    std::vector<std::string> worklist;
    worklist.push_back(entryFilePath);

    while (!worklist.empty()) {
        std::string currentFile = worklist.back();
        worklist.pop_back();

        if (std::find(processedFiles.begin(), processedFiles.end(), currentFile) != processedFiles.end()) {
            continue; // Already processed
        }

        std::string source = loader.loadSource(currentFile);
        if (source.empty()) {
            continue; // Error message is printed by loader
        }

        processedFiles.push_back(currentFile);

        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        Parser parser(tokens);
        std::vector<NodePtr> nodes = parser.parse();

        for (const auto& node : nodes) {
            if (auto importNode = std::dynamic_pointer_cast<ImportNode>(node)) {
                fs::path parentPath = fs::path(currentFile).parent_path();
                fs::path importPath = parentPath / importNode->path;

                if (importPath.extension() == ".cmod") {
                    fs::path tempDir = fs::temp_directory_path() / importPath.stem();
                    if (CMODSystem::unpack(importPath.string(), tempDir.string())) {
                        fs::path mainChtlFile = tempDir / "src" / (importPath.stem().string() + ".chtl");
                        if (fs::exists(mainChtlFile)) {
                            worklist.push_back(fs::absolute(mainChtlFile).string());
                        } else {
                            std::cerr << "Error: Main CHTL file not found in module: " << mainChtlFile << std::endl;
                        }
                    } else {
                        std::cerr << "Error: Failed to unpack CMOD file: " << importPath << std::endl;
                    }
                } else {
                     worklist.push_back(fs::absolute(importPath).string());
                }

            } else if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(node)) {
                templateTable[templateNode->name] = templateNode;
                // We don't add the template definition itself to the main AST for generation
            }
            else if(node) { // Parser can return nullptrs for skipped tokens
                ast.push_back(node);
            }
        }
    }

    // All files are parsed and the AST is built.
    // Now, generate the output.

    Generator generator(ast, templateTable);
    std::string html = generator.generate();
    std::cout << html << std::endl;
}

} // namespace CHTL

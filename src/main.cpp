#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: chtl <input.chtl> [--parse]\n";
		return 1;
	}

	bool runParse = false;
	std::string inputPath;
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--parse") { runParse = true; }
		else { inputPath = arg; }
	}

	std::ifstream in(inputPath);
	if (!in) {
		std::cerr << "Failed to open: " << inputPath << "\n";
		return 1;
	}

	std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	chtl::Lexer lexer(source);
	if (!runParse) {
		std::vector<chtl::Token> tokens;
		for (;;) {
			auto tok = lexer.nextToken();
			tokens.push_back(tok);
			if (tok.type == chtl::TokenType::EndOfFile) break;
		}
		for (const auto& t : tokens) {
			std::cout << chtl::toString(t.type) << " \t" << t.lexeme << "\n";
		}
		return 0;
	}

	chtl::Parser parser(std::move(lexer));
	auto doc = parser.parse();
	std::cout << "Parsed document" << "\n";

	return 0;
}


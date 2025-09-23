#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "lexer/Lexer.hpp"

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: chtl <input.chtl>\n";
		return 1;
	}

	const std::string inputPath = argv[1];
	std::ifstream in(inputPath);
	if (!in) {
		std::cerr << "Failed to open: " << inputPath << "\n";
		return 1;
	}

	std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	chtl::Lexer lexer(source);
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


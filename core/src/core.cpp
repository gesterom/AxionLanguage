// core.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <cassert>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "CodeLocation.h"
#include "Enumerate.h"
#include "MetaLexer.h"
#include "PreambleDefinition.h"
#include "StringUtility.h"

int main(int argc, char** args)
{
	if (argc < 2) return -1;
	for (int i = 0; i < argc; i++) {
		std::cout << i << " : " << args[i] << std::endl;
	}
	//std::ifstream file(args[1]);
	PreambleRepository repo;
	//for (int file_count = 1; file_count < argc; file_count++) {// FIXME -o makes output 
	MetaLexer lexer(repo, args[1/*file_count*/]);
	uint64_t i = 0;
	while (auto a = lexer.lex()) {
		if (a->kind == Token::Type::comment and a->preamble_token == -1) continue;
		std::cout << ++i << " : (" << repo.to_string(a->preamble_token) << "/" << repo.to_string(a->preamble_token, a->kind) << ") " << (a->value) << " -> " << a->value.start() << "-" << a->value.end() << std::endl;
	}
	//}
}

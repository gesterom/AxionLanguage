// core.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <cassert>
#include <functional>
#include <type_traits>
#include <optional>

#include "Enumerate.h"
#include "StringUtility.h"
#include "CodeLocation.h"
#include "MetaLexer.h"
#include "PreambleDefinition.h"

int main(int argc,char** args)
{
	if (argc < 2) return - 1;
	//std::ifstream file(args[1]);
	PreambleRepository repo;
	MetaLexer lexer(repo,args[1]);
	uint64_t i = 0;
	while (auto a = lexer.lex()) {
		//if (a->kind == Token::Type::comment and a->preamble_token == -1) continue;
		std::cout<<++i<<" : (" <<repo.to_string(a->preamble_token)<<"/" << repo.to_string(a->preamble_token,a->kind) << ") " << (a->value) << " -> "<<a->value.start() << "-"<<a->value.end() << std::endl;
	}
}

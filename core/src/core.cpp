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

struct CodeFragment {
	PreambleDefinition preamble;
	std::map<std::string, std::string> atributes;
	std::vector<CodeLocation> body;
	std::string head;
};

struct Lexem {
	enum class Type
	{
		unknown = 0,
		id,
		preamble,
		keyword,
		parathetis,
		op,
		semicolon,
		colon,
		comma,
		string_literal,
		number_literal,
		double_literal,
		char_literal,
		comment,
	};
	Type type = Type::unknown;
	CodeLocation val;
	Lexem(std::string filename) : val(filename) {}
};

bool eachAnd(std::string str,std::function<bool(uint8_t a)> func) {
	for (const auto& a : str) {
		if (not func(a)) return false;
	}
	return true;
}

bool isDoubleliteral(const std::string& a) {
	bool dot = false;
	for (const auto& i : a) {
		if (i == '.' and dot) return false;
		else if (i == '.' and not dot) { dot = true; }
		else if (not (std::isdigit(i) or (i >= 'a' and i<='z' or i >= 'A' and i <= 'Z') or i == '_')) return false;
	}
	return true;
}

int main(int argc,char** args)
{
	if (argc < 2) return - 1;
	//std::ifstream file(args[1]);
	PreambleRepository repo;
	MetaLexer lexer(repo,args[1]);
	uint64_t i = 0;
	while (auto a = lexer.lex()) {
		if (a->kind == Token::Type::comment and a->preamble_token == -1) continue;
		std::cout<<++i<<" : (" <<repo.to_string(a->preamble_token)<<"/" << repo.to_string(a->preamble_token,a->kind) << ") " << (a->value) << " -> "<<a->value.start() << "-"<<a->value.end() << std::endl;
	}
}

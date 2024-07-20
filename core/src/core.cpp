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
		std::cout<<++i<<" : (" <<repo.to_string(a->preamble_token)<<"/" << repo.to_string(a->preamble_token,a->kind) << ") " << (a->value) << " -> "<<a->value.start() << "-"<<a->value.end() << std::endl;
	}
	//auto a = metaLexer(args[1]);
	//for (const auto& [i, elem] : enumerate(a)) {
	//	std::cout<<"--- === "<<std::setw(3)<<i<<" === ---"<<std::endl;
	//	std::cout<<"Atributes:"<<std::endl;
	//	for (const auto& [j, atr] : enumerate(elem.atributes)) {
	//		std::cout<<"\t"<<std::setw(5)<<j<<": "<<atr.first.val()<<" = " << atr.second.val()<<std::endl;
	//	}

	//	std::cout<<"Preamble : " << std::setw(5) <<elem.preamble.start()<<" - "<< std::setw(5) << elem.preamble.end()<<" = " << elem.preamble.val() << std::endl;
	//	std::cout << "Head : " << std::setw(5) << elem.head.start() << " - " << std::setw(5) << elem.head.end() << " = " << elem.head.val() << std::endl;
	//	//std::cout << "Body : " << std::setw(5) << elem.body.start() << " - " << std::setw(5) << elem.body.end() << " = " << elem.body.val() << std::endl;
	//	for (const auto& [j, line] : enumerate(elem.lines)) {
	//		std::cout << std::setw((int)std::ceil(log10(elem.lines.size()))) << std::setw(5) << line.start() << " - " << std::setw(5) << line.end() << ":" << std::setw(30) << "(TODO) - " << line.val() << std::endl;
	//	}
	//}
}

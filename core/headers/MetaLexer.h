#pragma once

#include <vector>
#include <fstream>
#include <optional>
#include "Enumerate.h"
#include "CodeLocation.h"
#include "Token.h"
#include "PreambuleRepository.h"

enum class LexerMode
{
	idle = 0,comment, atribute_name, atribute_value, file_atribute_name, file_atribute_value, preambule, head, body,
};

class MetaLexer {
	CodeLocation loc;
	std::ifstream file;
	char last_ch = '\0';
	char ch = '\0';
	char next_ch = '\0';
	LexerMode mode = LexerMode::idle;
	int64_t preambuleIndex = -1;

	const PreambuleRepository& repo;

public:
	MetaLexer(const PreambuleRepository& repo,std::string file_name) ;
	std::optional<Token> lex();
};
#pragma once

#include <vector>
#include <fstream>
#include <optional>
#include "Enumerate.h"
#include "CodeLocation.h"
#include "Token.h"
#include "PreambleRepository.h"

enum class LexerMode
{
	idle = 0, comment, atribute_name, atribute_value, file_atribute_name, file_atribute_value, preamble, head, body,
};

class MetaLexer {
	CodeLocation loc;
	std::ifstream file;
	char last_ch = '\0';
	char ch = '\0';
	char next_ch = '\0';
	LexerMode mode = LexerMode::idle;
	int64_t preambleIndex = -1;

	const PreambleRepository& repo;

public:
	MetaLexer(const PreambleRepository& repo,std::string file_name) ;
	std::optional<Token> lex();
};
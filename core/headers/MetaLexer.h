#pragma once

#include "CodeLocation.h"
#include "PreambleRepository.h"
#include "Token.h"
#include <optional>

enum class LexerMode
{
	idle = 0, error_recovery, comment, string_literal, multilineComment, atribute_name, atribute_value, file_atribute_name, file_atribute_value, preamble, head, head_specific, body, body_specific, emit_body_closing_bracker
};

class MetaLexer {
	CodeLocation loc;
	LexerMode mode = LexerMode::idle;
	LexerMode nextMode = LexerMode::idle;
	LexerMode afterComment = LexerMode::idle;
	int64_t commentLevel = 0;
	int32_t preambleIndex = -1;

	const PreambleRepository& repo;

	CodeLocation tempLoc;

	int64_t parentisisCounter = 0;
	int64_t bodyComment = 0;
	bool inComment = false;
	bool inQuote = false;
	uint8_t start_string_lieral = '\0';

public:
	MetaLexer(const PreambleRepository& repo, std::string file_name);
	std::optional<Token> lex();
};
#pragma once

#include <vector>
#include "AST.h"
#include "MetaLexer.h"
#include "PreambleRepository.h"

class MetaParser
{
	PreambleRepository& repo;
	public:
		MetaParser(PreambleRepository& pre_repo);
		std::optional<PreambleNode> parseProgram(MetaLexer& lexer);
};


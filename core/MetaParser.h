#pragma once

#include <vector>
#include "AST.h"
#include "MetaLexer.h"
#include "SyntaxRepository.h"

class MetaParser
{
	SyntaxRepository& repo;
	public:
		MetaParser(SyntaxRepository& pre_repo);
		std::optional<PreambleNode> parseProgram(MetaLexer& lexer);
};


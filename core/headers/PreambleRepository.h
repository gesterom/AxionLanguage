#pragma once

#include "NodeBuilder.h"
#include "Preambles/Procedure/OperatorRepository.h"
#include "Token.h"
#include <string>	
#include <vector>

class PreambleDefinition;

using PreamleIndex = uint32_t;

class PreambleRepository {
	std::vector<PreambleDefinition*> vec;
	Preamble::Procedure::OperatorRepository repo;
	NodeBuilder nodeBuilder;
public:
	PreambleRepository();
	std::vector<PreambleDefinition*> get() const;
	PreambleDefinition* get(PreamleIndex index) const;
	PreamleIndex getPeambuleIndex(CodeLocation representation) const;
	std::string to_string(Token t) const;
	std::string to_string(PreamleIndex preambleIndex, Token::Type type) const;
	std::string prambleName(Token pre) const;
	std::string NodeKind(PreamleIndex preambleIndex, uint64_t n)const;
	~PreambleRepository();
};

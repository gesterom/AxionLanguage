#pragma once

#include "Token.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>


struct Ast {

	using NodeIndex = std::pair<uint32_t, uint32_t>;
	struct Node
	{
		uint64_t kind = 0;
		std::vector<Ast::NodeIndex> children; // nodeType(in witch vector to search) and nodeId
	};
	std::optional<NodeIndex> headNode = std::nullopt;
	std::optional<NodeIndex> bodyNode = std::nullopt;
	std::vector<Token> leafs; // nodeType == 0
	std::vector<Node> nodes; // nodeType == 1
	//std::vector<uint32_t> numbers; // nodeType == 1
};



struct PreambleNode {
	std::map<Token, Token> atributes;
	Token preambleKind;
	Ast ast;
};

void cleanAst(Ast& ast);
std::ostream& operator<<(std::ostream& out, const std::optional<Ast::NodeIndex>& a);
class IParser;
std::ostream& ast_to_string(std::ostream& out, IParser* p, Ast& ast);

/*
	1. std::vector<Preamble> <- highest level
	Preamble <- contains Ast
*/
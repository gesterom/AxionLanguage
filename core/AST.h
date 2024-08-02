#pragma once

#include <vector>
#include <string>

#include "Token.h"


struct Ast {
	
	using NodeIndex = std::pair<uint32_t, uint32_t>;
	static const std::pair<uint32_t, uint32_t> ErrorNode;
	struct Node
	{
		uint64_t kind = 0 ;
		std::vector<Ast::NodeIndex> children; // nodeType(in witch vector to search) and nodeId
	};
	uint64_t headNode = 0;
	uint64_t bodyNode = 0;
	std::vector<Token> leafs; // nodeType == 0
	std::vector<uint32_t> numbers; // nodeType == 1
	std::vector<Node> nodes; // nodeType == 2
	NodeIndex newLeaf(Token t);
	NodeIndex newNode(Node t);
	NodeIndex newNumber(uint32_t t);
};



/*
	1. std::vector<Preamble> <- highest level
	Preamble <- contains Ast
*/
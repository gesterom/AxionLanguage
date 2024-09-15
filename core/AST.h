#pragma once

#include "SyntaxRepository.h"
#include "Token.h"
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Ast {

	using NodeIndex = std::pair<uint32_t, uint32_t>;
	struct Node
	{
		uint32_t kind = 0;
		std::vector<Ast::NodeIndex> children; // nodeType(in witch vector to search) and nodeId
	};
	std::optional<NodeIndex> headNode = std::nullopt;
	std::optional<NodeIndex> bodyNode = std::nullopt;
	std::vector<Token> leafs; // nodeType == 0
	std::vector<Node> nodes; // nodeType == 1
	//std::vector<uint32_t> numbers; // nodeType == 1
	CodeLocation span(Ast::NodeIndex index) const;
};

bool isLeaf(Ast::NodeIndex);
bool isLeaf(std::optional<Ast::NodeIndex>);

class IParser;
struct PreambleNode {
	std::map<Token, std::optional<Token>> atributes;
	Token preambleKind;
	Ast ast;
	IParser* parser;
};

void cleanAst(Ast& ast);
std::ostream& operator<<(std::ostream& out, const std::optional<Ast::NodeIndex>& a);
//std::ostream& ast_to_string(std::ostream& out, IParser* p, Ast& ast);
std::string astToGraph(const PreambleNode& preamble, const SyntaxRepository& repo);

void astTraversal(
	Ast ast,
	std::optional<Ast::NodeIndex> root,
	std::function<void(Ast, Ast::NodeIndex)> preVisitExecute,
	std::function<void(Ast, Ast::NodeIndex)> postVisitExecute
);
#pragma once

#include "AST.h"
#include "Result.h"
#include <string>
#include <unordered_map>
#include <vector>

class NodeBuilder
{
public:
	struct ChildDescription {
		std::string name;
		uint64_t nodeKind;
	};
private:
	Ast* ast = nullptr;
	std::unordered_map<uint64_t, std::string> stringRepo;
	std::unordered_map<uint64_t, std::vector<ChildDescription>> rules;
	std::unordered_map<uint64_t, bool> repetable;
	std::unordered_map<uint64_t, std::vector<uint64_t>> polimorfizm;
public:
	void setAst(Ast* ast);
	Ast::NodeIndex newLeaf(Result<Token, ErrorT>);
	uint64_t addNodeKind(std::string name, std::vector<ChildDescription>, bool repetable = false);
	uint64_t addInharitedNodeKind(std::string name, uint64_t parent, std::vector<ChildDescription> rule, bool repeat = false);
	Ast::NodeIndex createNode(uint64_t kind, std::vector<Ast::NodeIndex>) const;
	std::string toString(uint64_t nodeKind) const;
};
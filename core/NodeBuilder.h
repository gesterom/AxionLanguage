#pragma once

#include "AST.h"
#include "Result.h"
#include <string>
#include <unordered_map>
#include <vector>

using NodeKindId = uint32_t;
class NodeBuilder
{
public:
	struct ChildDescription {
		std::string name;
		NodeKindId nodeKind;
	};
private:
	Ast* ast = nullptr;
	std::unordered_map<NodeKindId, std::string> stringRepo;
	std::unordered_map<NodeKindId, std::vector<ChildDescription>> rules;
	std::unordered_map<NodeKindId, bool> repetable;
	std::unordered_map<NodeKindId, std::vector<NodeKindId>> polimorfizm;
public:
	void setAst(Ast* ast);
	Ast::NodeIndex newLeaf(Result<Token, ErrorT>);
	NodeKindId addNodeKind(std::string name, std::vector<ChildDescription>, bool repetable = false);
	NodeKindId addInharitedNodeKind(std::string name, NodeKindId parent, std::vector<ChildDescription> rule, bool repeat = false);
	Ast::NodeIndex createNode(NodeKindId kind, std::vector<Ast::NodeIndex>) const;
	std::string toString(NodeKindId nodeKind) const;
};
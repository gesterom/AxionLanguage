#pragma once

#include "AST.h"
#include "SyntaxRepository.h"
#include "Result.h"
#include <string>
#include <unordered_map>
#include <vector>

class NodeBuilder
{
	Ast* ast;
	SyntaxRepository& repo;
	std::map<uint32_t, NodeKindIndex> translator;
public:
	using ExternalNodeId = uint32_t;
	NodeBuilder(SyntaxRepository& repo,Ast* ast = nullptr);
	Ast::NodeIndex createLeaf(Result<Token, ErrorT>)const;
	Ast::NodeIndex createNode(ExternalNodeId kind, std::vector<Ast::NodeIndex>) const;
	Ast& getAst() const;
	void setAst(Ast*);

	NodeKindIndex addNodeKind(ExternalNodeId externalEnum, std::string name, std::vector<SyntaxRepository::ChildDescription>, bool repetable = false);
	NodeKindIndex addIPolimorficNodeKind(ExternalNodeId externalEnum, std::string name, NodeKindIndex parent, std::vector<SyntaxRepository::ChildDescription> rule, bool repeat = false);
	void addLeafToPolimorficNodeKind(std::string name, ExternalNodeId parent);
	std::string nodeKind(ExternalNodeId nodeKind)const;
	std::string nodeKindChilden(ExternalNodeId nodeKind, uint32_t childrenIndex)const;

};
#pragma once

#include "AST.h"
#include "Result.h"
#include "SyntaxRepository.h"
#include <string>
#include <unordered_map>
#include <vector>

template<typename T>
void printError(const Result<T, ErrorT>& err) {
	if (not err)
		std::cout << "ERROR : " << ((ErrorT)err).loc.start() << "  -> " << ((ErrorT)err).oneLinerError << std::endl;
}

void printError(const std::optional<ErrorT>& err);

class NodeBuilder
{
	Ast* ast;
	SyntaxRepository& repo;
	std::map<uint32_t, NodeKindIndex> translator;
public:
	using ExternalNodeId = uint32_t;
	NodeBuilder(SyntaxRepository& repo, Ast* ast = nullptr);
	Ast::NodeIndex createLeaf(const Result<Token, ErrorT>&)const;
	Ast::NodeIndex createNode(ExternalNodeId kind, std::vector<Ast::NodeIndex>) const;
	Ast& getAst() const;
	void setAst(Ast*);

	NodeKindIndex addNodeKind(ExternalNodeId externalEnum, std::string name, std::vector<SyntaxRepository::ChildDescription>, bool repetable = false);
	NodeKindIndex addPolimorficNodeKind(ExternalNodeId externalEnum, std::string name, ExternalNodeId parent, std::vector<SyntaxRepository::ChildDescription> rule, bool repeat = false);
	void addLeafToPolimorficNodeKind(std::string name, ExternalNodeId parent);
	std::string nodeKind(ExternalNodeId nodeKind)const;
	std::string nodeKindChilden(ExternalNodeId nodeKind, uint32_t childrenIndex)const;

	NodeKindIndex translate(ExternalNodeId nodeKind);

};
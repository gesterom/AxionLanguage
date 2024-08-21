#pragma once

#include "Preambles/Procedure/OperatorRepository.h"
#include "Token.h"
#include <string>	
#include <vector>
#include <unordered_map>

class PreambleDefinition;

using PreamleIndex = uint32_t;
using NodeKindIndex = uint32_t;

class SyntaxRepository {
public:
	
	struct ChildDescription {
		std::string name;
		NodeKindIndex nodeKind;
	};
private:
	std::vector<PreambleDefinition*> vec;
	OperatorRepository operatorRepo;
	std::unordered_map<NodeKindIndex, std::string> nodeKindNames;
	std::unordered_map<NodeKindIndex, std::vector<ChildDescription>> nodeConstructionRules;
	std::unordered_map<NodeKindIndex, bool> nodeKindRuleRepatable;
	std::unordered_map<NodeKindIndex, std::vector<NodeKindIndex>> nodePolimorfizm;
public:
	SyntaxRepository();
	std::vector<PreambleDefinition*> get() const;
	PreambleDefinition* get(PreamleIndex index) const;

	NodeKindIndex addNodeKind(std::string name, std::vector<ChildDescription>, bool repetable = false);
	NodeKindIndex addPolimorficNodeKind(std::string name, NodeKindIndex parent, std::vector<ChildDescription> rule, bool repeat = false);
	void addLeafToPolimorficNodeKind(std::string name, NodeKindIndex parent);
	std::string nodeKind(NodeKindIndex n)const;
	std::string nodeKindChilden(NodeKindIndex nodeKind, uint64_t childrenIndex)const;
	bool isRepetableNodeRule(NodeKindIndex)const;
	std::optional<std::vector<NodeKindIndex>> polimorficTypes(NodeKindIndex)const;
	std::vector<ChildDescription> nodeKindRule(NodeKindIndex)const;

	OperatorRepository operators() const;

	PreamleIndex getPeambuleIndex(CodeLocation representation) const;
	//change to_string so it will be using same mechanism as NodeKind/NodeBuilder
	//1. Register token type
	//2. use internal count of tokens
	//
	std::string to_string(Token t) const;
	std::string to_string(PreamleIndex preambleIndex, Token::Type type) const;
	std::string prambleName(Token pre) const;
	~SyntaxRepository();
};

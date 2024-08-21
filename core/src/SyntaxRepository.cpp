#include "SyntaxRepository.h"

#include "Preambles/Function/Lexer.h"
#include "Preambles/Procedure/Lexer.h"
#include "Preambles/Type/Lexer.h"		


#include "Preambles/Procedure/Parser.h"

#include "PreambleDefinition.h"

#include "format"
#include <Enumerate.h>

std::optional<Token> nop_lexer(int64_t preambleIndex, LexerMode& mode, CodeLocation& loc, std::istream& in) {
	return std::nullopt;
}

std::string SyntaxRepository::prambleName(Token pre) const
{
	if (pre.kind <= Token::count) return "<Meta>";
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (pre.preambleIndex < (int64_t)(vec.size() - 1 % 100000)) return vec.at(pre.preambleIndex)->representation;
	return "<Incorect Preamble Index>";
}

std::string SyntaxRepository::nodeKind(NodeKindIndex nodeKind) const
{
	if (nodeKind == 0) return "Leaf";
	auto it = nodeKindNames.find(nodeKind);
	if (it == nodeKindNames.end())
		UNREACHABLE("Node kind not registered");
	return it->second;
}

std::string SyntaxRepository::nodeKindChilden(NodeKindIndex nodeKind, uint64_t childrenIndex) const
{
	auto it = nodeConstructionRules.find(nodeKind);
	ASSERT(it != nodeConstructionRules.end(), std::format("Kind not registered : {}", nodeKind));
	return it->second[childrenIndex % it->second.size()].name;
}

bool SyntaxRepository::isRepetableNodeRule(NodeKindIndex kind) const
{
	auto it = nodeKindRuleRepatable.find(kind);
	if (it != nodeKindRuleRepatable.end())
		return it->second;
	UNREACHABLE(std::format("Kind not registered : {}", kind));
}

std::optional<std::vector<NodeKindIndex>> SyntaxRepository::polimorficTypes(NodeKindIndex kind) const
{
	auto it = nodePolimorfizm.find(kind);
	if (it != nodePolimorfizm.end())
		return it->second;
	return std::nullopt;
}

std::vector<SyntaxRepository::ChildDescription> SyntaxRepository::nodeKindRule(NodeKindIndex kind) const
{
	auto it = nodeConstructionRules.find(kind);
	if (it != nodeConstructionRules.end())
		return it->second;
	UNREACHABLE(std::format("Kind not registered : {}", kind));
}

OperatorRepository SyntaxRepository::operators() const
{
	return this->operatorRepo;
}

void SyntaxRepository::addLeafToPolimorficNodeKind(std::string name, NodeKindIndex parent) {
	auto it = nodeConstructionRules.find(parent);
	ASSERT(it != nodeConstructionRules.end(), std::format("Kind not registered : {}", parent));
	ASSERT(it->second.size() == 0, std::format("Parent can have only 0 size {}", this->nodeKind(parent)));
	nodePolimorfizm[parent].push_back(0);
}

NodeKindIndex SyntaxRepository::addPolimorficNodeKind(std::string name, NodeKindIndex parent, std::vector<ChildDescription> rule, bool repeat)
{
	auto it = nodeConstructionRules.find(parent);
	ASSERT(it != nodeConstructionRules.end(), std::format("Kind not registered : {}", parent));
	ASSERT(it->second.size() == 0, std::format("Parent can have only 0 size {}", this->nodeKind(parent)));

	NodeKindIndex newid = (NodeKindIndex)nodeKindNames.size() + 1;
	nodeKindNames.emplace(newid, name);
	nodeConstructionRules.emplace(newid, rule);
	nodeKindRuleRepatable.emplace(newid, repeat);
	nodePolimorfizm[parent].push_back(newid);
	return newid;
}

NodeKindIndex SyntaxRepository::addNodeKind(std::string name, std::vector<ChildDescription> rule, bool repeat)
{
	NodeKindIndex newid = (NodeKindIndex)nodeKindNames.size() + 1;
	nodeKindNames.emplace(newid, name);
	nodeConstructionRules.emplace(newid, rule);
	nodeKindRuleRepatable.emplace(newid, repeat);
	return newid;
}

std::string meta_to_string(Token::Type t)
{
	switch (t)
	{
	case Token::error: return "error";
	case Token::unknown: return "unknown";
	case Token::preamble: return "preamble";
	case Token::file_scope_atribute_name: return "file_scope_atribute_name";
	case Token::file_scope_atribute_value: return "file_scope_atribute_value";
	case Token::atribute_name: return "atribute_name";
	case Token::atribute_value: return "atribute_value";
	case Token::parenthesis: return "parenthesis";
	case Token::comment: return "comment";
	case Token::string_literal: return "string_literal";
	case Token::atom: return "atom";
	case Token::count: return "Invalid :\"Count\"";
	default: return "<invalid>";
	}
}

std::string SyntaxRepository::to_string(Token t) const
{
	if (t.kind <= Token::count) return meta_to_string(t.kind);
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (t.preambleIndex < (int64_t)(vec.size() % 100000)) {
		ASSERT(vec.at(t.preambleIndex)->lexer != nullptr, "Internal Error : Lexer shoude be init");
		return vec.at(t.preambleIndex)->lexer->to_string(t.kind);
	}
	return "<Incorect Preamble Index>";
}

std::string SyntaxRepository::to_string(PreamleIndex preambleIndex, Token::Type type) const
{
	if (type <= Token::count) return meta_to_string(type);
	else if (vec.size() - 1 > 100000) TODO("Too much preambles");
	else if (preambleIndex < (int64_t)(vec.size() % 100000)) {
		//ASSERT(vec.at(preambleIndex)->lexer != nullptr, "Internal Error : Lexer shoude be init");
		if (vec.at(preambleIndex)->lexer == nullptr) return "<Lexer nil "+std::to_string(type)+">";
		return vec.at(preambleIndex)->lexer->to_string(type);
	}
	return "<Incorect Preamble Index>";
}

SyntaxRepository::SyntaxRepository()
{
	auto addPreamble = [&](PreambleDefinition* pd) {
		vec.push_back(pd);
		if (vec.at(vec.size() - 1)->lexer != nullptr)
			vec.at(vec.size() - 1)->lexer->setPreambleIndex((int32_t)vec.size() - 1);
		};
	addPreamble(new PreambleDefinition{ "<Meta>" });
	addPreamble(new PreambleDefinition{ "extension" }); // load something like type or agents or sal // extends syntax of core language
	addPreamble(new PreambleDefinition{ "procedure",new Preamble::Procedure::Lexer(operatorRepo),new Preamble::Procedure::Parser(*this) });
	addPreamble(new PreambleDefinition{ "function",new Preamble::Function::Lexer() });
	addPreamble(new PreambleDefinition{ "type",new Preamble::Type::Lexer() });
	addPreamble(new PreambleDefinition{ "type.distinct",new Preamble::Type::Lexer() });
	addPreamble(new PreambleDefinition{ "type.alias",new Preamble::Type::Lexer() });
	addPreamble(new PreambleDefinition{ "type.interface",nullptr }); //
	addPreamble(new PreambleDefinition{ "build",new Preamble::Procedure::Lexer(operatorRepo),new Preamble::Procedure::Parser(*this) }); // runed to compile similar to nobuild(https://github.com/tsoding/nobuild)
	//addPreamble(new PreambleDefinition{ "build.procedure",nullptr }); // build time procedure
	addPreamble(new PreambleDefinition{ "sql",nullptr }); // sql scrip
	addPreamble(new PreambleDefinition{ "sql.query",nullptr }); // sql quer
	addPreamble(new PreambleDefinition{ "sql.migration",nullptr });
}

std::vector<PreambleDefinition*> SyntaxRepository::get() const
{
	std::vector<PreambleDefinition*> res;
	for (auto i : vec) {
		res.push_back(i);
	}
	return res;
}

PreambleDefinition* SyntaxRepository::get(PreamleIndex index) const
{
	ASSERT(vec.size() < 100000, "TOO large");
	ASSERT(index >= 0 && index < (int64_t)vec.size(), "out of bound");
	return vec[index];
}

PreamleIndex SyntaxRepository::getPeambuleIndex(CodeLocation representation) const {

	for (int32_t i = 0; i < vec.size(); i++) {
		if (vec.at(i)->representation == representation) {
			return i;
		}
	}
	TODO(std::format("preamble '{}' not found ! in {} ", representation.to_string(), representation.start()));
}

SyntaxRepository::~SyntaxRepository()
{
	for (auto i : vec) {
		delete i;
	}
}

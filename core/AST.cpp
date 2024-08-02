#include "AST.h"

const std::pair<uint32_t, uint32_t> Ast::ErrorNode = { -1,-1 };

Ast::NodeIndex Ast::newLeaf(Token t)
{
	auto res = (uint32_t)this->leafs.size();
	this->leafs.push_back(t);
	return {0,res};
}

Ast::NodeIndex Ast::newNumber(uint32_t t)
{
	auto res = (uint32_t)this->numbers.size();
	this->numbers.push_back(t);
	return { 1,res };
}

Ast::NodeIndex Ast::newNode(Ast::Node t)
{
	auto res = (uint32_t)this->nodes.size();
	this->nodes.push_back(t);
	return { 2,res };
}
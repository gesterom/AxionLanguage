#include "NodeBuilder.h"

#include "TODO.h"
#include <format>

void NodeBuilder::setAst(Ast* ast)
{
	this->ast = ast;
}

void printError(const std::optional<ErrorT>& err) {
	if (err.has_value()) {
		std::cout << "ERROR : " << err->loc.start() << " : " << err->loc.to_string() << " = " << err.value().oneLinerError << std::endl;
	}
}

Ast::NodeIndex NodeBuilder::newLeaf(Result<Token, ErrorT> t)
{
	if (t) {
		uint32_t res = ast->leafs.size();
		ast->leafs.push_back((Token)t);
		return { 0,res };
	}
	else {
		printError((ErrorT)t);
		UNREACHABLE("ERROR");
	}
}

uint64_t NodeBuilder::addNodeKind(std::string name, std::vector<ChildDescription> rule, bool repeat)
{
	uint64_t newid = stringRepo.size() + 1;
	stringRepo.emplace(newid, name);
	rules.emplace(newid, rule);
	repetable.emplace(newid, repeat);
	return newid;
}

uint64_t NodeBuilder::addInharitedNodeKind(std::string name, uint64_t parent, std::vector<ChildDescription> rule, bool repeat)
{
	auto it = rules.find(parent);
	ASSERT(it != rules.end(), std::format("Kind not registered : {}", parent));
	ASSERT(it->second.size() == 0, std::format("Parent can have only 0 size {}", this->toString(parent)));

	uint64_t newid = stringRepo.size() + 1;
	stringRepo.emplace(newid, name);
	rules.emplace(newid, rule);
	repetable.emplace(newid, repeat);
	polimorfizm[parent].push_back(newid);
	return newid;
}

Ast::NodeIndex NodeBuilder::createNode(uint64_t kind, std::vector<Ast::NodeIndex> elements) const
{
	auto it = rules.find(kind);
	ASSERT(it != rules.end(), std::format("Kind not registered : {}", kind));
	if (repetable.at(kind)) {
		ASSERT(elements.size() % it->second.size() == 0, std::format("Repetable node can hold only hold multiple of nodes: `{}`", this->toString(kind)));
	}
	else {
		ASSERT(it->second.size() == elements.size(), std::format("wrong elements number for node kind `{}`", this->toString(kind)));
	}

	for (size_t i = 0; i < elements.size(); i++) {
		size_t index = i % it->second.size();

		if (polimorfizm.find(it->second[index].nodeKind) != polimorfizm.end()) {
			bool any = false;
			for (size_t polI = 0; polI < polimorfizm.at(it->second[index].nodeKind).size(); polI++) {
				if (elements[i].first == polimorfizm.at(it->second[index].nodeKind)[polI]) { any = true; break; }
			}
			ASSERT(any, std::format("Node kind dont match expected {} get {} at position {}", this->toString(it->second[index].nodeKind), this->toString(elements[i].first), i));
		}
		else {
			ASSERT(elements[i].first == it->second[index].nodeKind, std::format("Node kind dont match expected `{}` get `{}` at position `{}`", this->toString(it->second[index].nodeKind), this->toString(elements[i].first), i));
		}
	}
	uint32_t res = ast->nodes.size();

	Ast::Node node;
	node.kind = kind;
	for (const auto& i : elements) {
		node.children.push_back(i);
	}
	ast->nodes.push_back(node);
	return { kind + 1,res };
}

std::string NodeBuilder::toString(uint64_t nodeKind) const
{
	auto it = stringRepo.find(nodeKind);
	if (it == stringRepo.end())
		UNREACHABLE("Node kind not registered");
	return it->second;
}
//
//std::ostream& operator<<(std::ostream& out, const std::optional<Ast::NodeIndex>& a)
//{
//	if (a.has_value()) {
//		out << std::format("({}:{})", a->first, a->second);
//	}
//	else {
//		out << std::format("(NULL)");
//	}
//	return out;
//}
//
//std::ostream& ast_to_string(std::ostream& out, IParser* p, Ast& ast)
//{
//	out << "Head : " << ast.headNode << std::endl;
//	out << "Body : " << ast.bodyNode << std::endl;
//	for (const auto& [i, n] : enumerate(ast.nodes)) {
//		out << "[" << i << "] Kind(" << p->NodeKind_toString(n.kind) << ") ";
//		for (const auto& j : n.children) {
//			if (j.first == 0) out << "Leaf(" << ast.leafs[j.second].value << ") ";
//			else out << j << " ";
//		}
//		out << std::endl;
//	}
//	return out;
//}
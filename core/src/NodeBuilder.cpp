#include "NodeBuilder.h"

#include "TODO.h"
#include <format>

void printError(const std::optional<ErrorT>& err) {
	if (err)
		std::cout << "ERROR : " << err->loc.start() << "  -> " << err->oneLinerError << std::endl;
}

void printError(const Result<Token, ErrorT>& err) {
	if (not err)
		std::cout << "ERROR : " << ((ErrorT)err).loc.start() << "  -> " << ((ErrorT)err).oneLinerError << std::endl;
}

NodeBuilder::NodeBuilder(SyntaxRepository& repo, Ast* ast) : repo(repo), ast(ast)
{

}

Ast::NodeIndex NodeBuilder::createLeaf(const Result<Token, ErrorT>& t) const
{
	ASSERT(ast != nullptr, "AST not set up");
	if (t) {
		uint32_t res = (uint32_t)ast->leafs.size();
		ast->leafs.push_back((Token)t);
		return { 0,res };
	}
	else {
		printError(t);
		UNREACHABLE("ERROR");
	}
}

Ast::NodeIndex NodeBuilder::createNode(ExternalNodeId _kind, std::vector<Ast::NodeIndex> elements) const
{
	ASSERT(ast != nullptr, "AST not set up");
	NodeKindIndex kind = translator.at(_kind);
	//fix it so it referes through a repo
	auto rule = repo.nodeKindRule(kind);
	if (repo.isRepetableNodeRule(kind)) {
		ASSERT(elements.size() % rule.size() == 0, std::format("Repetable node can hold only hold multiple of nodes: `{}`", repo.nodeKind(kind)));
	}
	else {
		ASSERT(elements.size() == rule.size(), std::format("wrong elements number for node kind `{}`", repo.nodeKind(kind)));
	}

	for (size_t i = 0; i < elements.size(); i++) {
		size_t index = i % rule.size();
		auto polIt = repo.polimorficTypes(rule[index].nodeKind);

		if (polIt) {

			bool any = false;
			for (size_t polI = 0; polI < polIt->size(); polI++) {
				if (elements[i].first == polIt.value()[polI]) { any = true; break; }
			}
			ASSERT(any, std::format("Node kind dont match expected polimorfed `{}` get `{}` at position {}", repo.nodeKind(rule[index].nodeKind), repo.nodeKind(elements[i].first), i));
		}
		else {
			ASSERT(elements[i].first == rule[index].nodeKind, std::format("Node kind dont match expected `{}` get `{}` at position `{}`", repo.nodeKind(rule[index].nodeKind), repo.nodeKind(elements[i].first), i));
		}
	}
	uint32_t res = (uint32_t)ast->nodes.size();

	Ast::Node node;
	node.kind = kind;
	for (const auto& i : elements) {
		node.children.push_back(i);
	}
	ast->nodes.push_back(node);
	return { kind,res };
}

Ast& NodeBuilder::getAst() const
{
	ASSERT(ast != nullptr, "Null ptr dereference");
	return *(this->ast);
}

void NodeBuilder::setAst(Ast* _ast)
{
	this->ast = _ast;
}

NodeKindIndex NodeBuilder::addNodeKind(ExternalNodeId externalEnum, std::string name, std::vector<SyntaxRepository::ChildDescription> rule, bool repetable) {
	std::vector<SyntaxRepository::ChildDescription> translated;
	for (const auto& i : rule) {
		if (i.nodeKind == 0) {
			translated.push_back(SyntaxRepository::ChildDescription{ i.name,0 });
		}
		else {
			translated.push_back(SyntaxRepository::ChildDescription{ i.name,translator[i.nodeKind] });
		}
	}
	translator.emplace(externalEnum, repo.addNodeKind(name, translated, repetable));
	return translator[externalEnum];
}
NodeKindIndex NodeBuilder::addPolimorficNodeKind(ExternalNodeId externalEnum, std::string name, ExternalNodeId parent, std::vector<SyntaxRepository::ChildDescription> rule, bool repetable) {
	std::vector<SyntaxRepository::ChildDescription> translated;
	for (const auto& i : rule) {
		if (i.nodeKind == 0) {
			translated.push_back(SyntaxRepository::ChildDescription{ i.name,0 });
		}
		else {
			translated.push_back(SyntaxRepository::ChildDescription{ i.name,translator[i.nodeKind] });
		}
	}
	translator.emplace(externalEnum, repo.addPolimorficNodeKind(name, translator[parent], translated, repetable));
	return translator[externalEnum];
}
void NodeBuilder::addLeafToPolimorficNodeKind(std::string name, ExternalNodeId parent) {
	repo.addLeafToPolimorficNodeKind(name, translator[parent]);
}
std::string NodeBuilder::nodeKind(ExternalNodeId nodeKind)const {
	return repo.nodeKind(translator.at(nodeKind));
}
std::string NodeBuilder::nodeKindChilden(ExternalNodeId nodeKind, uint32_t childrenIndex)const {
	return repo.nodeKindChilden(translator.at(nodeKind), childrenIndex);
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
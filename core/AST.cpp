#include "AST.h"

#include "TODO.h"
#include <format>
#include <sstream>

void cleanAst(Ast& ast)
{
	TODO("this is too complicated for now");
}

std::ostream& operator<<(std::ostream& out, const std::optional<Ast::NodeIndex>& a)
{
	if (a.has_value()) {
		out << std::format("({}:{})", a->first, a->second);
	}
	else {
		out << std::format("(NULL)");
	}
	return out;
}

std::string astToGraph(const PreambleNode& preamble,const SyntaxRepository& repo) {
	std::stringstream ss;
	ss << "head : " << preamble.ast.headNode << std::endl;
	ss << "body : " << preamble.ast.bodyNode << std::endl;
	ss << "digraph " << preamble.preambleKind.value.to_string() << "{" << std::endl;
	for (size_t i = 0; i < preamble.ast.nodes.size(); i++) {
		for (size_t j = 0; j < preamble.ast.nodes[i].children.size(); j++) {
			auto& child = preamble.ast.nodes[i].children[j];
			ss << "\t" << "Node_"<< preamble.ast.nodes[i].kind<<"_" << i << " -> " << "Node_" << child.first << "_" << child.second << "[label=\"" << repo.nodeKindChilden(preamble.ast.nodes[i].kind,j) << "\"]" << std::endl;
		}
	}
	for (size_t i = 0; i < preamble.ast.leafs.size(); i++) {
		ss << "\t" << "Node_0_" << i << "[label=\"" << preamble.ast.leafs[i].value.to_string() << "\"]" << std::endl;
	}
	for (size_t i = 0; i < preamble.ast.nodes.size(); i++) {
		ss << "\t" << "Node_" << preamble.ast.nodes[i].kind << "_" << i << "[label=\"" << repo.nodeKind(preamble.ast.nodes[i].kind) << "\",shape=record]" << std::endl;
	}
	ss << "}";
	return ss.str();
}

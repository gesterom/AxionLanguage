#include "AST.h"

#include "Enumerate.h"
#include "PreambleDefinition.h"
#include "TODO.h"
#include <format>
#include <sstream>

void cleanAst(Ast& ast)
{
	TODO("this is too complicated for now");
}

std::string astToGraph(const PreambleNode& preamble,const SyntaxRepository& repo) {
	std::stringstream ss;
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

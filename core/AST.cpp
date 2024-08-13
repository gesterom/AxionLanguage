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

std::ostream& ast_to_string(std::ostream& out, IParser* p, Ast& ast)
{
	out << "Head : " << ast.headNode << std::endl;
	out << "Body : " << ast.bodyNode << std::endl;
	for (const auto& [i, n] : enumerate(ast.nodes)) {
		out << "[" << i << "] Kind(" << p->NodeKind_toString(n.kind) << ") ";
		for (const auto& j : n.children) {
			if (j.first == 0) out << "Leaf(" << ast.leafs[j.second].value << ") ";
			else out << j << " ";
		}
		out << std::endl;
	}
	return out;
}

std::string astToGraph(const PreambleNode& preamble, IParser* p) {
	std::stringstream ss;
	ss << "digraph "<<preamble.preambleKind.value.to_string() << "{"<<std::endl;
	for (size_t i =0 ; i < preamble.ast.nodes.size() ;i ++) {
		for (const auto& child : preamble.ast.nodes[i].children) {
			ss << "\t" << "Node_1_"<<i<<" -> "<<"Node_"<< child.first<<"_"<<child.second<<std::endl;
		}
	}
	for (size_t i = 0; i < preamble.ast.leafs.size(); i++) {
		ss<<"\t"<<"Node_0_"<<i<<"[label=\""<< preamble.ast.leafs[i].value.to_string()<<"\"]"<<std::endl;
	}
	for (size_t i = 0; i < preamble.ast.nodes.size(); i++) {
		ss << "\t" << "Node_1_" << i << "[label=\"" << p->NodeKind_toString(preamble.ast.nodes[i].kind) << "\"]" << std::endl;
	}
	ss<<"}";
	return ss.str();
}

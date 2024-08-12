#include "AST.h"

#include "Enumerate.h"
#include "PreambleDefinition.h"
#include "TODO.h"
#include <format>

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

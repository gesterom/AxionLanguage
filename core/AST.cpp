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

std::string astToGraph(const PreambleNode& preamble, const SyntaxRepository& repo) {
	std::stringstream ss;
	ss << "head : " << preamble.ast.headNode << std::endl;
	ss << "body : " << preamble.ast.bodyNode << std::endl;
	ss << "digraph " << preamble.preambleKind.value.to_string() << "{" << std::endl;
	for (size_t i = 0; i < preamble.ast.nodes.size(); i++) {
		for (size_t j = 0; j < preamble.ast.nodes[i].children.size(); j++) {
			auto& child = preamble.ast.nodes[i].children[j];
			ss << "\t" << "Node_" << preamble.ast.nodes[i].kind << "_" << i << " -> " << "Node_" << child.first << "_" << child.second << "[label=\"" << repo.nodeKindChilden(preamble.ast.nodes[i].kind, j) << "\"]" << std::endl;
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

CodeLocation Ast::span(Ast::NodeIndex index) const
{
	std::optional<CodeLocation> res;
	std::vector<Ast::NodeIndex> stack;
	stack.push_back(index);
	while (stack.size() > 0) {
		auto current = stack[stack.size() - 1];
		stack.pop_back();
		if (current.first == 0) /*leaf*/ {
			if (not res.has_value()) {
				res = this->leafs[current.second].value;
			}
			else {
				res = res->combaine(this->leafs[current.second].value);
			}
		}
		else /*node*/ {
			for (const auto& child : this->nodes[current.second].children) {
				stack.push_back(child);
			}
		}
	}
	if (res)
		return res.value(); // if it dont have any tokens inside (how it was created)
	return CodeLocation::null();
}

bool isLeaf(Ast::NodeIndex n) {
	return n.first == 0;
}

bool isLeaf(std::optional<Ast::NodeIndex> n) {
	if (n.has_value()) {
		return isLeaf(n.value());
	}
	return false;
}

void astTraversal(
	Ast ast,
	std::optional<Ast::NodeIndex> root,
	std::function<void(Ast, Ast::NodeIndex)> preVisitExecute,
	std::function<void(Ast, Ast::NodeIndex)> postVisitExecute
) {
	if (not root)
		return;

	std::vector<std::pair<Ast::NodeIndex, uint64_t>> stack;

	// Store an index for each node in the stack to track which child to process
	stack.push_back(std::make_pair(root.value(), 0));  // The pair is (node, childIndex)

	while (not stack.empty()) {
		auto& [node, childIndex] = *(stack.end() - 1);

		// Preorder visit (preVisitExecute) is executed when first visiting the node
		if (childIndex == 0) {
			preVisitExecute(ast, node);
		}

		// Check if all children are processed
		if (childIndex < ast.nodes[node.second].children.size() and not isLeaf(node)) {
			// Push the next child onto the stack
			auto nextChild = ast.nodes[node.second].children[childIndex];
			stack.pop_back();  // Pop the current state
			stack.push_back(std::make_pair(node, childIndex + 1));  // Update with incremented child index
			stack.push_back(std::make_pair(nextChild, 0));  // Push the child with its own index 0 (starting to process)
		}
		else {
			// Postorder visit (postVisitExecute) is executed when done with all children
			postVisitExecute(ast, node);
			stack.pop_back();  // Done with this node
		}
	}
}
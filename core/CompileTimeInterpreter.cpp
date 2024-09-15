#include "CompileTimeInterpreter.h"

#include "Preambles/Procedure/Lexer.h"
#include "Preambles/Procedure/Parser.h"
#include "TODO.h"

using NodeKinds = Preamble::Procedure::NodeKinds;

//knowTypes.push_back(TypeInfo("double")); 1
//knowTypes.push_back(TypeInfo("bool")); 2
//knowTypes.push_back(TypeInfo("int")); 3
//knowTypes.push_back(TypeInfo("char")); 4

CompileTimeInterpreter::RuntimeExpression::RuntimeExpression(int v)
{
	this->type = 3;
	this->val.int_val = v;
}

CompileTimeInterpreter::RuntimeExpression::RuntimeExpression(double v)
{
	this->type = 1;
	this->val.double_val = v;
}

CompileTimeInterpreter::RuntimeExpression::RuntimeExpression(bool v)
{
	this->type = 2;
	this->val.bool_val = v;
}

CompileTimeInterpreter::RuntimeExpression::RuntimeExpression(Runtime_Type v)
{
	this->type = v;
	this->val.type_val = v;
}

CompileTimeInterpreter::RuntimeExpression::RuntimeExpression(CompileTimeInterpreter& context, Runtime_Function v)
{
	this->type = context.knowFunctions[v].type;
	this->val.function_val = v;
}

auto CompileTimeInterpreter::preVisit(PreambleNode& code, SyntaxRepository& repo) {
	return [&](Ast ast, Ast::NodeIndex node) -> void {
		//if (not isLeaf(node)) {
		//	std::cout << "Pre (" << node.first << "," << node.second << ") = " <<
		//		repo.nodeKind(
		//			ast.nodes[node.second].kind
		//		) << std::endl;
		//}

		if (isLeaf(node)) return;
		else if (ast.nodes[node.second].kind == code.parser->translate(NodeKinds::varible_declaration)) {
			auto var_name = ast.leafs[ast.nodes[node.second].children[0].second];
			auto var_type = ast.nodes[ast.nodes[node.second].children[1].second];
			auto var_init_expr = ast.nodes[ast.nodes[node.second].children[2].second];
			std::cout << "Pre " << ast.leafs[ast.nodes[node.second].children[0].second].value.to_string() << std::endl;
		}
		};
}
auto CompileTimeInterpreter::postVisit(PreambleNode& code, SyntaxRepository& repo) {
	return [&](Ast ast, Ast::NodeIndex node) -> void {
		//if (not isLeaf(node)) {
		//	std::cout << "Post (" << node.first << "," << node.second << ") = " <<
		//		repo.nodeKind(
		//			ast.nodes[node.second].kind
		//		) << std::endl;
		//}

		if (isLeaf(node)) {
			if (ast.leafs[node.second].kind == (Token::Type)Preamble::Procedure::ProcedureTokenType::integer_literal) {
				RuntimeExpression t = RuntimeExpression{ std::stoi(ast.leafs[node.second].value.to_string()) };
				lastExpressionValue.push_back(t);
			}
			else if (ast.leafs[node.second].kind == (Token::Type)Preamble::Procedure::ProcedureTokenType::bool_literal) {
				if (ast.leafs[node.second].value.to_string() == "True") {
					RuntimeExpression t = RuntimeExpression{ true };
					lastExpressionValue.push_back(t);
				}
				else if (ast.leafs[node.second].value.to_string() == "False") {
					RuntimeExpression t = RuntimeExpression{ false };
					lastExpressionValue.push_back(t);
				}
				else {
					TODO("Compile time errors");
				}
			}
			else if (ast.leafs[node.second].kind == (Token::Type)Preamble::Procedure::ProcedureTokenType::double_literal) {
				RuntimeExpression t = RuntimeExpression{ std::stod(ast.leafs[node.second].value.to_string()) };
				lastExpressionValue.push_back(t);
			}
			else if (ast.leafs[node.second].kind == Token::Type::atom)
			{

			}
			return;
		}
		else if (ast.nodes[node.second].kind == code.parser->translate(NodeKinds::infix_operator)) {
			auto t = code.parser->translate(NodeKinds::infix_operator);
			ASSERT(lastExpressionValue.size() >= 2, "TODO compile time error");
			auto op = ast.leafs[ast.nodes[node.second].children[0].second];
			auto right = lastExpressionValue[lastExpressionValue.size() - 1];
			auto left = lastExpressionValue[lastExpressionValue.size() - 2];
			if (knowTypes[left.type].type != TypeInfo::Type::buildIn) return;
			if (knowTypes[left.type].name != "int") return;
			if (knowTypes[right.type].type != TypeInfo::Type::buildIn) return;
			if (knowTypes[right.type].name != "int") return;

			if (op.value.to_string() == "+") {
				lastExpressionValue.pop_back();
				lastExpressionValue.pop_back();
				RuntimeExpression t = RuntimeExpression{ left.val.int_val + right.val.int_val };
				lastExpressionValue.push_back(t);
			}
			else if (op.value.to_string() == "*") {
				lastExpressionValue.pop_back();
				lastExpressionValue.pop_back();
				RuntimeExpression t = RuntimeExpression{ left.val.int_val * right.val.int_val };
				lastExpressionValue.push_back(t);
			}
		}
		else if (ast.nodes[node.second].kind == code.parser->translate(NodeKinds::varible_declaration)) {
			std::cout << "Post val : ";
			if (lastExpressionValue.size() == 0) {
				std::cout << "<EMPTY>" << std::endl;
				return;
			}
			auto lastOne = lastExpressionValue.at(lastExpressionValue.size() - 1);
			auto Type = knowTypes[lastOne.type];
			// TODO deduce type of declaration
			//if (ast.nodes[node.second].children[1].first == code.parser->translate(NodeKinds::empty_expression)) {
			//	// i dont know how to convert RuntimeType into CompiletimeType yet
			//}
			if (Type.type == TypeInfo::Type::buildIn) {
				if (Type.name == "int") {
					std::cout << Type.name << " = " << lastOne.val.int_val;
				}
				else if (Type.name == "double") {
					std::cout << Type.name << " = " << lastOne.val.double_val;
				}
				else if (Type.name == "bool") {
					std::cout << Type.name << " = " << ((lastOne.val.bool_val) ? ("True") : ("False"));
				}
				else {
					std::cout << Type.name << " = " << "TODO print type val";
				}
			}
			std::cout << std::endl;
		}
		else if (ast.nodes[node.second].kind == code.parser->translate(NodeKinds::function_call)) {

		}
		};
}

CompileTimeInterpreter::CompileTimeInterpreter()
{
	knowTypes.push_back(TypeInfo("__void"));
	knowTypes.push_back(TypeInfo("double"));
	knowTypes.push_back(TypeInfo("bool"));
	knowTypes.push_back(TypeInfo("int"));
	knowTypes.push_back(TypeInfo("char"));
	knowTypes.push_back(TypeInfo(TypeInfo::Type::arrayType, "string", 4));
	knowTypes.push_back(TypeInfo("__ char[]->void", TypeInfo::FunctTypeDef{ 5,0 }));
	knowFunctions.push_back(FunctionInfo{ "compile", 6, Ast{} });
}

void CompileTimeInterpreter::execute(PreambleNode& code, SyntaxRepository& repo)
{
	if (code.preambleKind.value != "build") return;
	if (not code.ast.headNode.has_value()) return;
	if (not code.ast.bodyNode.has_value()) return;
	if (isLeaf(code.ast.headNode)) {
		std::cout << "Compiler internal Error" << std::endl;
	}
	if (code.ast.nodes[code.ast.headNode->second].kind != code.parser->translate(Preamble::Procedure::NodeKinds::function_head)) return;

	std::cout << "[BUILD]" << std::endl;
	auto name = code.ast.nodes[code.ast.headNode->second].children[0];
	auto scope = code.ast.nodes[name.second].children[0];
	if (code.ast.nodes[scope.second].kind != code.parser->translate(Preamble::Procedure::empty_scope_path)) return;
	auto coreNameIndex = code.ast.nodes[name.second].children[1];
	if (not isLeaf(coreNameIndex)) return;
	auto coreName = code.ast.leafs[coreNameIndex.second];
	// 1. find `build all()`;
	if (coreName.value != "all") return;
	// TODO if all(string[] args) present populate it with compilere arguments
	astTraversal(code.ast, code.ast.bodyNode, this->preVisit(code, repo), this->postVisit(code, repo));
}

CompileTimeInterpreter::TypeInfo::TypeInfo(std::string n) {
	this->type = CompileTimeInterpreter::TypeInfo::Type::buildIn;
	this->name = n;
	this->def.buildin_val = nullptr;
}
CompileTimeInterpreter::TypeInfo::TypeInfo(std::string n, CompileTimeInterpreter::TypeInfo::RecordDef rec) {
	this->type = CompileTimeInterpreter::TypeInfo::Type::structType;
	this->name = n;
	this->def.structdef_val = rec;
}
CompileTimeInterpreter::TypeInfo::TypeInfo(std::string n, CompileTimeInterpreter::TypeInfo::FunctTypeDef func) {
	this->type = CompileTimeInterpreter::TypeInfo::Type::functionType;
	this->name = n;
	this->def.funcdef_val = func;
}
CompileTimeInterpreter::TypeInfo::TypeInfo(CompileTimeInterpreter::TypeInfo::Type t, std::string n, CompileTimeInterpreter::Runtime_Type innner) {
	this->type = t;
	this->name = n;
	this->def.innerType = innner;
}

CompileTimeInterpreter::TypeInfo::~TypeInfo()
{
	if (this->type == Type::structType) {
		delete def.structdef_val.fields;
	}
}

CompileTimeInterpreter::TypeInfo::TypeDef::TypeDef()
{
	this->buildin_val = nullptr;
}

CompileTimeInterpreter::TypeInfo::TypeDef::~TypeDef()
{

}

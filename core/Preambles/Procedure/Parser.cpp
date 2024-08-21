#include "Preambles/Procedure/Parser.h"

#include "Preambles/Procedure/Lexer.h"
#include <format>
#include <iostream>

using namespace Preamble::Procedure;

Result<Ast::NodeIndex, ErrorT> Parser::requireName(TokenStream& head, Ast& ast) {
	std::vector<Token> atoms;
	auto t = head.require(Token::Type::atom);
	if (not t) {
		return (ErrorT)t;
	}
	atoms.emplace_back(t);
	while (head.optional((Token::Type)ProcedureTokenType::operator_t, ".")) {
		auto t2 = head.require(Token::Type::atom);
		if (not t2) {
			return ErrorT(t2);
		}
		atoms.emplace_back(t2);
	}
	if (atoms.size() == 0) return ErrorT{ head.peak(-1).value().value,"expected id","" };
	Token last = atoms.at(atoms.size() - 1);
	std::vector<Ast::NodeIndex> path;
	for (int i = 0; i < atoms.size() - 1; i++) {
		path.push_back(builder.createLeaf(atoms[i]));
	}
	if(path.size() == 0){
		auto path_id = builder.createNode(NodeKinds::empty_scope_path, {});
		return builder.createNode(NodeKinds::name, { path_id,builder.createLeaf(last) });
	}else{
		auto path_id = builder.createNode(NodeKinds::scope_path, path);
		return builder.createNode(NodeKinds::name, { path_id,builder.createLeaf(last) });
	}
}

bool checkIsAtomcExpresion(TokenStream& head) {
	auto t = head.peak(0);
	if (not t.has_value()) return false;
	if (t->kind == Token::Type::atom) return true;
	if (t->kind == Token::Type::string_literal) return true;
	if (t->kind == (Token::Type)ProcedureTokenType::integer_literal) return true;
	if (t->kind == (Token::Type)ProcedureTokenType::double_literal) return true;
	return false;
}

Result<Ast::NodeIndex, ErrorT> Parser::parsePrimary(TokenStream& head, Ast& ast) {
	if (checkIsAtomcExpresion(head)) {
		auto t = head.peak();
		head.consume();
		return builder.createLeaf(t.value());
	}
	return ErrorT{ head.peak(0)->value,std::format("Expected primary Expresion get {}",head.peak()->value.to_string()),"TODO long error" };
}

Result<Ast::NodeIndex, ErrorT> Parser::parseExpressionListOperator(TokenStream& head, Ast& ast, NodeKinds kind, std::string endToken, std::vector<Ast::NodeIndex>& output) {
	std::vector<Ast::NodeIndex> temp;
	do {
		auto a = parseExpresion(head, ast);
		if (a) temp.push_back(a);
		else break;
	} while (head.optional((Token::Type)ProcedureTokenType::comma));
	auto t = head.require(Token::Type::parenthesis, endToken);
	if (not t) return (ErrorT)t;
	auto args = builder.createNode(NodeKinds::expression_list, temp);
	Ast::NodeIndex func = output[output.size() - 1];
	output.pop_back();;
	std::vector<Ast::NodeIndex> funcCall;
	funcCall.push_back(func);
	funcCall.push_back(args);
	output.push_back(builder.createNode(NodeKinds::function_call, funcCall));
	return output[output.size() - 1];
}

std::optional<Token> isPrimaryExpresion(TokenStream& head) {
	if (not head.peak().has_value()) return std::nullopt;
	auto k = head.peak()->kind;
	if (k == Token::Type::atom
		or k == Token::Type::string_literal
		or k == (Token::Type)ProcedureTokenType::bool_literal
		or k == (Token::Type)ProcedureTokenType::double_literal
		or k == (Token::Type)ProcedureTokenType::integer_literal
		) {
		auto t = head.peak();
		head.consume();
		return t;
	}
	return std::nullopt;
}

bool isOperator(const Ast& ast, Ast::NodeIndex index) {
	if (index.first != 0) return false; // needs to be leaf
	//if (ast.nodes[index.second].children.size() == 0) return false; // needs to have atleast 1 children 
	Token op = ast.leafs[index.second];
	if (op.kind == (Token::Type)ProcedureTokenType::operator_t) return true;
	return false;
}

bool isValue(const Ast& ast, Ast::NodeIndex index) {
	return not isOperator(ast, index);
}

bool isPrefix(const OperatorRepository& repo, Ast& ast, std::optional<Ast::NodeIndex> index) {
	if (index == std::nullopt) return false;
	if (isValue(ast, index.value())) return false;
	return repo.isPrefix(ast.leafs[index->second].value.to_string());
}

bool isInfix(const OperatorRepository& repo, Ast& ast, std::optional<Ast::NodeIndex> index) {
	if (index == std::nullopt) return false;
	if (isValue(ast, index.value())) return false;
	return repo.isInfix(ast.leafs[index->second].value.to_string());
}

bool isSuffix(const OperatorRepository& repo, Ast& ast, std::optional<Ast::NodeIndex> index) {
	if (index == std::nullopt) return false;
	if (isValue(ast, index.value())) return false;
	return repo.isSuffix(ast.leafs[index->second].value.to_string());
}

std::optional<Ast::NodeIndex> get(const std::vector<Ast::NodeIndex>& vec, int32_t index) {
	if (index >= vec.size()) return std::nullopt;
	if (index < 0) return std::nullopt;
	return vec[index];
}

void Parser::reduce_output(const OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& last, int32_t new_precedence) {
	while (true)
	{
		if (
			last.size() > 1 and
			isOperator(ast, last[last.size() - 1]) and
			isValue(ast, last[last.size() - 2]) and
			isSuffix(repo, ast, last[last.size() - 1])) { //SUFFUX operator
			auto pre = repo.getPrecedenceSuffix(ast.leafs[last[last.size() - 1].second].value.to_string());
			auto left = repo.isleftAssociativitySuffix(ast.leafs[last[last.size() - 1].second].value.to_string());
			if (pre < new_precedence or (pre == new_precedence and left)) {
				std::vector<Ast::NodeIndex> res;
				res.push_back(last[last.size() - 1]);
				res.push_back(last[last.size() - 2]);
				last.pop_back(); last.pop_back();
				last.push_back(builder.createNode(NodeKinds::suffix_operator, res));
				continue;
			}
		}
		else if (
			last.size() > 2 and
			isValue(ast, last[last.size() - 1]) and
			isOperator(ast, last[last.size() - 2]) and
			isValue(ast, last[last.size() - 3]) and
			isInfix(repo, ast, last[last.size() - 2])) { // INFIX operator
			auto pre = repo.getPrecedenceInfix(ast.leafs[last[last.size() - 2].second].value.to_string());
			auto left = repo.isleftAssociativityInfix(ast.leafs[last[last.size() - 2].second].value.to_string());
			if (pre < new_precedence or (pre == new_precedence and left)) {
				std::vector<Ast::NodeIndex> res;
				res.push_back(last[last.size() - 2]);
				res.push_back(last[last.size() - 3]);
				res.push_back(last[last.size() - 1]);
				last.pop_back(); last.pop_back(); last.pop_back();
				last.push_back(builder.createNode(NodeKinds::infix_operator, res));
				continue;
			}
		}
		else if (
			last.size() > 1 and
			isValue(ast, last[last.size() - 1]) and
			isOperator(ast, last[last.size() - 2]) and
			isPrefix(repo, ast, last[last.size() - 2]))
		{ // PREFIX operator
			auto pre = repo.getPrecedencePrefix(ast.leafs[last[last.size() - 2].second].value.to_string());
			auto left = repo.isleftAssociativityPrefix(ast.leafs[last[last.size() - 2].second].value.to_string());
			if (pre < new_precedence or (pre == new_precedence and left)) {
				std::vector<Ast::NodeIndex> res;
				res.push_back(last[last.size() - 2]);
				res.push_back(last[last.size() - 1]);
				last.pop_back(); last.pop_back();
				last.push_back(builder.createNode(NodeKinds::prefix_operator, res));
				continue;
			}
		}
		break;
	}
}

std::optional<CodeLocation> min(Ast& ast, Ast::NodeIndex index) {
	if (index.first == 0) return ast.leafs[index.second].value;
	if (ast.nodes[index.second].children.size() == 0) return std::nullopt;
	auto res = min(ast, ast.nodes[index.second].children[0]);
	for (size_t i = 1; i < ast.nodes[index.second].children.size(); i++) {
		auto temp = min(ast, ast.nodes[index.second].children[i]);
		if (not res.has_value()) res = temp;

		if (temp.has_value() and temp->file_pointer_start() < res->file_pointer_start())
			res = temp;
	}
	return res;
}

OperatorPrecedence Parser::deducePrecedence(const OperatorRepository& repo, Ast& ast, std::vector<Ast::NodeIndex>& output, std::string representation) {
	ASSERT(output.size() > 0, "output size > 0; parseExpression as caller");

	int32_t p1 = -1;
	int32_t p2 = -1;
	int32_t p3 = -1;

	if (isValue(ast, output[output.size() - 1])) {
		p2 = repo.getPrecedenceInfix(representation);
		p3 = repo.getPrecedenceSuffix(representation);
	}
	if (isPrefix(repo, ast, output[output.size() - 1]) and repo.isPrefix(representation)) {
		p1 = repo.getPrecedencePrefix(representation);
	}
	if (isInfix(repo, ast, output[output.size() - 1]) and repo.isPrefix(representation)) {
		p1 = repo.getPrecedencePrefix(representation);
	}
	if (isSuffix(repo, ast, output[output.size() - 1]) and repo.isInfix(representation)) {
		p2 = repo.getPrecedenceInfix(representation);
	}
	if (isSuffix(repo, ast, output[output.size() - 1]) and repo.isSuffix(representation)) {
		p3 = repo.getPrecedenceSuffix(representation);
	}
	int32_t pre = 0;
	if (p1 != -1 and (p1 >= p2 or p2 == -1) and (p1 >= p3 or p3 == -1))
		pre = p1;
	if (p2 != -1 and (p2 >= p1 or p1 == -1) and (p2 >= p3 or p3 == -1))
		pre = p2;
	if (p3 != -1 and (p3 >= p2 or p2 == -1) and (p3 >= p1 or p1 == -1))
		pre = p3;
	ASSERT(pre != -1, "TODO : Syntax error expresion");
	return pre;
}

// let a = vector(int){nullptr,0,0};
Result<Ast::NodeIndex, ErrorT> Parser::parseExpresion(TokenStream& head, Ast& ast) {
	// not Token?
	//Ast::Node res;
	//res.kind = (uint64_t)NodeKinds::expression;
	std::vector<Ast::NodeIndex> output;
	int32_t precedence = 0;

	bool lastIsValue = false;
	while (true) {
		//value
		if (not lastIsValue and head.optional(Token::Type::parenthesis, "(")) { // brackets
			auto e = parseExpresion(head, ast);
			if (e) output.push_back(e);
			auto t = head.require(Token::Type::parenthesis, ")");
			if (not t) return (ErrorT)t;
			lastIsValue = true;
			continue;
		}
		else if (not lastIsValue and head.optional(Token::Type::parenthesis, "[")) { //array literal
			std::vector<Ast::NodeIndex> temp;
			do {
				auto a = parseExpresion(head, ast);
				if (a) temp.push_back(a);
				else break;
			} while (head.optional((Token::Type)ProcedureTokenType::comma));
			auto t = head.require(Token::Type::parenthesis, "]");
			output.push_back(builder.createNode(NodeKinds::array_literal, temp));
			lastIsValue = true;
			continue;
		}
		else if (lastIsValue and head.optional(Token::Type::parenthesis, "(")) { //function_call or tuple
			parseExpressionListOperator(head, ast, NodeKinds::function_call, ")", output);
			lastIsValue = true;
			continue;
		} //  
		else if (lastIsValue and head.optional(Token::Type::parenthesis, "{")) { //constructor
			parseExpressionListOperator(head, ast, NodeKinds::Object_construct, "}", output);
			lastIsValue = true;
			continue;
		}
		else if (lastIsValue and head.optional(Token::Type::parenthesis, "[")) { //array acess
			parseExpressionListOperator(head, ast, NodeKinds::array_acess, "]", output);
			lastIsValue = true;
			continue;
		}
		else if (auto t = isPrimaryExpresion(head)) {
			output.emplace_back(builder.createLeaf(t.value()));
			lastIsValue = true;
			continue;
		}
		//operators
		else if (auto t = head.optional((Token::Type)::ProcedureTokenType::operator_t)) {
			if (output.size() > 0) {
				auto pre = deducePrecedence(repo.operators(), ast, output, t->value.to_string());
				reduce_output(repo.operators(), ast, output, pre);
			}
			output.emplace_back(builder.createLeaf(t.value()));
			lastIsValue = false;
			continue;
		}
		break;//if token not handled stop parsing expression
	}
	reduce_output(repo.operators(), ast, output, INT32_MAX);
	if (output.size() == 1) {
		return (Ast::NodeIndex)output[0];
	}
	else if (output.size() == 0) {
		return builder.createNode(NodeKinds::empty_expression, {});
	}
	else {
		auto err = min(ast, output[1]);
		ASSERT(err != std::nullopt, "something fishy!!");
		if (err) {
			return ErrorT{ err.value(),std::format("Unexpected token {}, invalid expresion !!",err->to_string()),"TODO long error" };
		}
		else
			UNREACHABLE("To remove warning");
	}
}

// atom : type/*expresion that evaluated in compile time will result in Type*/
Result<Ast::NodeIndex, ErrorT> Parser::requireFunctionHeadArgs(TokenStream& head, Ast& ast) {
	std::vector<Ast::NodeIndex> res;
	if (not head.check(Token::Type::parenthesis, ")")) {
		do {
			res.push_back(builder.createLeaf(head.require(Token::Type::atom)));
			head.require((Token::Type)ProcedureTokenType::colon);
			res.push_back(parseExpresion(head, ast));
		} while (head.optional((Token::Type)ProcedureTokenType::comma));
	}
	return builder.createNode(NodeKinds::function_head_args_definition, res);
}

// try stmt -> expresion = to stmt or return from function with error

// name args returnType
std::optional<Ast::NodeIndex> Parser::parseHead(TokenStream& head, Ast& ast) {
	std::vector<ErrorT> errors;
	std::vector<Ast::NodeIndex> res;
	res.resize(3);
	if (auto name = requireName(head, ast)) {
		res[0] = name; // name
	}
	else { return std::nullopt; }
	if (not head.require(Token::Type::parenthesis, "(")) return std::nullopt;
	auto args = requireFunctionHeadArgs(head, ast);
	if (not args) return std::nullopt;
	res[1] = args;

	if (not head.require(Token::Type::parenthesis, ")")) return std::nullopt;

	if (head.optional((Token::Type)ProcedureTokenType::operator_t, "->")) {
		auto retT = parseExpresion(head, ast);
		if (not retT)
			res[2] = retT;
		else return std::nullopt;
	}
	else {
		res[2] = builder.createNode(NodeKinds::empty_expression, {});
	}
	if (head.requireEmpty()) return std::nullopt; //TODO Better error : "unexpected tockens"
	return builder.createNode(NodeKinds::function_head, res);
}

std::optional<Ast::NodeIndex> Parser::parseBody(TokenStream& body, Ast& ast) {
	body.require(Token::Type::parenthesis, "{");
	auto t = parseExpresion(body, ast);
	body.require(Token::Type::parenthesis, "}");
	if (t) return t;
	else {std::cout << ((ErrorT)t).loc.start() << "  -> " << ((ErrorT)t).oneLinerError << std::endl; return std::nullopt; }
}

Preamble::Procedure::Parser::Parser(SyntaxRepository& repo) : repo(repo), builder(repo)
{
	builder.addNodeKind(NodeKinds::scope,"scope", { } );
	builder.addIPolimorficNodeKind(NodeKinds::empty_scope_path,"empty_scope_path", NodeKinds::scope, { });
	builder.addIPolimorficNodeKind(NodeKinds::scope_path,"scope_path", NodeKinds::scope, { {"path",NodeKinds::leaf} }, true);
	builder.addNodeKind(NodeKinds::name,"name", { {"path",NodeKinds::scope},{"core_name",NodeKinds::leaf} });
	builder.addNodeKind(NodeKinds::expression,"expression", {});
	builder.addNodeKind(NodeKinds::expression_list,"expression_list", { {"element" , NodeKinds::expression} }, true);
	builder.addNodeKind(NodeKinds::function_head_args_definition,"function_head_args_definition", { {"argument_name",NodeKinds::leaf},{"type",NodeKinds::expression} }, true);
	builder.addNodeKind(NodeKinds::function_head,"function_head", { {"function_name",NodeKinds::name}, {"Args definition",NodeKinds::function_head_args_definition}, {"ReturnType",NodeKinds::expression} });

	builder.addIPolimorficNodeKind(NodeKinds::empty_expression,	"empty_expression",	NodeKinds::expression, {});
	builder.addIPolimorficNodeKind(NodeKinds::function_call,	"function_call",	NodeKinds::expression, { { "Function",NodeKinds::expression},{"Parameters", NodeKinds::expression_list} });
	builder.addIPolimorficNodeKind(NodeKinds::array_literal,	"array_literal",	NodeKinds::expression, { { "Elements", NodeKinds::expression_list } });
	builder.addIPolimorficNodeKind(NodeKinds::Object_construct,	"Object_construct",	NodeKinds::expression, { { "Constructed Type",NodeKinds::expression},{"Parameters", NodeKinds::expression_list} });
	builder.addIPolimorficNodeKind(NodeKinds::array_acess,		"array_acess",		NodeKinds::expression, { { "Array",NodeKinds::expression},{"indexes", NodeKinds::expression_list} });
	builder.addIPolimorficNodeKind(NodeKinds::prefix_operator,	"prefix_operator",	NodeKinds::expression, { { "Operator",NodeKinds::leaf},{"right", NodeKinds::expression} });
	builder.addIPolimorficNodeKind(NodeKinds::infix_operator,	"infix_operator",	NodeKinds::expression, { { "Operator",NodeKinds::leaf},{"left", NodeKinds::expression},{"right", NodeKinds::expression} });
	builder.addIPolimorficNodeKind(NodeKinds::suffix_operator,	"suffix_operator",	NodeKinds::expression, { { "Operator",NodeKinds::leaf},{"left", NodeKinds::expression} });
	builder.addLeafToPolimorficNodeKind("variable", NodeKinds::expression);
}

Ast Preamble::Procedure::Parser::parse(TokenStream& head, TokenStream& body)
{
	Ast ast;
	builder.setAst(&ast);
	ast.headNode = parseHead(head, ast);
	if (ast.headNode == std::nullopt) return ast;
	ast.bodyNode = parseBody(body, ast);
	builder.setAst(nullptr);
	return ast;
}

Preamble::Procedure::Parser::~Parser()
{

}

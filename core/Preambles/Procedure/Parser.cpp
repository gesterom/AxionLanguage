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

Result<Ast::NodeIndex, ErrorT> Preamble::Procedure::Parser::parseType(TokenStream& head, Ast& ast)
{
	// not Token?
	//Ast::Node res;
	//res.kind = (uint64_t)NodeKinds::expression;
	std::vector<Ast::NodeIndex> output;
	int32_t precedence = 0;

	bool lastIsValue = false;
	while (true) {
		//value
		if (not lastIsValue and head.optional(Token::Type::parenthesis, "(")) { // brackets
			auto e = parseType(head, ast);
			if (e) output.push_back(e);
			auto t = head.require(Token::Type::parenthesis, ")");
			if (not t) return (ErrorT)t;
			lastIsValue = true;
			continue;
		}
		else if (not lastIsValue and head.optional(Token::Type::parenthesis, "[")) { //array literal
			std::vector<Ast::NodeIndex> temp;
			do {
				auto a = parseType(head, ast);
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
		else if (head.peak()->value == "=" and head.peak()->kind == (Token::Type)::ProcedureTokenType::operator_t) {
			break;
		}
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
			return ErrorT{ err.value(),std::format("Unexpected token {}, invalid type expression !!",err->to_string()),"TODO long error" };
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
			res.push_back(parseType(head, ast));
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
		auto retT = parseType(head, ast);
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

// return expr catch expr ;
// evaluate first and if it is true
// evaluate expr1 
// if error return expr2
// fallthrough
// expr1? // will return value or short circute function
// expr1??expr2 // will return value or short circute function with returned value on right
// expr1?.expr2 // if left side is error it do nothing, if it is corect it will acess right side as member of expr1 and return expr2 or Error from expr1+eventual error from expr2
// so type of expr1?.expr2 is
// expr2 ? expr1.error+expr2.error
//

Result<Ast::NodeIndex, ErrorT> Parser::parseIf(TokenStream& body, Ast& ast) {
	auto startParam = body.require(Token::Type::atom, "if");
	if (not startParam) { printError(startParam); return ErrorT{body.peak()->value,"if statement needs to start with `if` keyword","TODO"}; }
	auto cond_expression = parseExpresion(body,ast);
	if (not cond_expression) { printError(cond_expression); return ErrorT{ body.peak()->value,"if statement require a expresion after `if` keyword","TODO" }; }
	auto thenKeyword = body.require((Token::Type)ProcedureTokenType::colon, ":");
	if (not thenKeyword) { printError(thenKeyword); return ErrorT{ body.peak()->value,"if statement require a `:` after condition","TODO" }; }
	
	auto if_true = parseStatement(body,ast);
	if (not if_true) { printError(if_true); return ErrorT{ body.peak()->value,"error in if statement body","TODO" }; }
	auto if_true_ = (Ast::NodeIndex)if_true;
	std::optional<Ast::NodeIndex> if_false =  std::nullopt;
	if (body.optional(Token::Type::atom, "else")) {
		auto falseBody = parseStatement(body, ast);
		if(not falseBody) printError(falseBody);
		else if_false = falseBody;
	}
	if (if_false.has_value()) {
		return builder.createNode(NodeKinds::if_branch, { cond_expression,if_true_,if_false.value()});
	}
	else {
		return builder.createNode(NodeKinds::if_branch, { cond_expression,if_true_,builder.createNode(NodeKinds::empty_stmt,{}) });
	}
}
Result<Ast::NodeIndex, ErrorT> Parser::parseLet(TokenStream& body, Ast& ast) {
	auto startParam = body.require(Token::Type::atom, "let");
	if (not startParam) { printError(startParam); return ErrorT{ body.peak()->value,"varible declaration statment needs to start with `let` keyword","TODO" }; }
	auto varName = body.require(Token::Type::atom);
	if (not varName) { printError(varName); return ErrorT{ body.peak()->value,"varible declaration statment needs varible name after `let` keyword","TODO" }; }
	std::optional<Ast::NodeIndex> type = std::nullopt;
	if (body.optional((Token::Type)ProcedureTokenType::colon, ":")) {
		//TODO parseExpression needs to stop on = 
		// now it will parse it and complayin about not reduced stack
		auto var_type = parseType(body,ast);
		if (not var_type) { printError(var_type); return ErrorT{ body.peak()->value,"varible declaration incorect type","TODO" }; }
		type = var_type;
	}
	auto assigne = body.require((Token::Type)ProcedureTokenType::operator_t,"=");
	if (not assigne) { printError(assigne); return ErrorT{ body.peak()->value,"varible declaration needs to initialize varible expected `=` ","TODO" }; }
	auto initValue = parseExpresion(body,ast);
	if (not initValue) { printError(initValue); return ErrorT{ body.peak()->value,"varible declaration needs to initialize varible","TODO" }; }
	auto endStmt = body.require((Token::Type)ProcedureTokenType::semicolon, ";");
	if (not endStmt) { printError(endStmt); return ErrorT{ body.peak()->value,"varible declaration needs to ends with `;`","TODO" }; }
	if (type) {
		return builder.createNode(NodeKinds::varible_declaration,{ builder.createLeaf(varName),type.value(),initValue});
	}
	else {
		return builder.createNode(NodeKinds::varible_declaration, { builder.createLeaf(varName),builder.createNode(NodeKinds::empty_expression,{}),initValue});
	}
}

Result<Ast::NodeIndex, ErrorT> Preamble::Procedure::Parser::parseStatement(TokenStream& body, Ast& ast)
{
	if (body.peak()->value == "if") {
		auto err = parseIf(body, ast);
		if (not err) {
			printError(err);
		}
		return err;
	}
	if (body.peak()->value == "let") {
		auto err = parseLet(body, ast);
		if (not err) {
			printError(err);
		}
		return err;
	}
	if (body.peak()->value == "{" and body.peak()->kind == Token::Type::parenthesis) {
		auto err = parseBlockStatement(body, ast);
		if (not err) {
			printError(err);
		}
		return err;
	}
	auto exprStmt = parseExpresion(body, ast);
	if (exprStmt) {
		auto endStmt = body.require((Token::Type)ProcedureTokenType::semicolon, ";");
		if (not endStmt) { printError(endStmt); return ErrorT{ body.peak()->value,"expression needs to ends with `;`","TODO" }; }
		return builder.createNode(NodeKinds::expression_stmt,{ exprStmt });
	}
	printError(exprStmt);
	return ErrorT{ body.peak()->value,"not recognized statement","TODO" };
}

Result<Ast::NodeIndex, ErrorT> Preamble::Procedure::Parser::parseBlockStatement(TokenStream& body, Ast& ast)
{
	auto startParam = body.require(Token::Type::parenthesis, "{");
	if (not startParam) { printError(startParam); return ErrorT{ body.peak()->value,"block statment starts with `{`","TODO" }; }
	std::vector<Ast::NodeIndex> inst;
	while(not body.optional(Token::Type::parenthesis, "}")){
		auto t = parseStatement(body, ast);
		if(not t)
			return ((ErrorT)t);
		else 
			inst.push_back((Ast::NodeIndex)t);
	}
	return builder.createNode(NodeKinds::stmt_block, inst);
}


std::optional<Ast::NodeIndex> Parser::parseBody(TokenStream& body, Ast& ast) {

	auto body_ast = parseBlockStatement(body,ast);
	if (not body_ast) {
		printError(body_ast);
		return std::nullopt;
	}
	return body_ast;
}

Preamble::Procedure::Parser::Parser(SyntaxRepository& repo) : repo(repo), builder(repo)
{
	builder.addNodeKind(NodeKinds::scope,"scope", { } );
	builder.addPolimorficNodeKind(NodeKinds::empty_scope_path,"empty_scope_path", NodeKinds::scope, { });
	builder.addPolimorficNodeKind(NodeKinds::scope_path,"scope_path", NodeKinds::scope, { {"path",NodeKinds::leaf} }, true);
	builder.addNodeKind(NodeKinds::name,"name", { {"path",NodeKinds::scope},{"core_name",NodeKinds::leaf} });
	builder.addNodeKind(NodeKinds::expression,"expression", {});
	builder.addNodeKind(NodeKinds::expression_list,"expression_list", { {"element" , NodeKinds::expression} }, true);
	builder.addNodeKind(NodeKinds::function_head_args_definition,"function_head_args_definition", { {"argument_name",NodeKinds::leaf},{"type",NodeKinds::expression} }, true);
	builder.addNodeKind(NodeKinds::function_head,"function_head", { {"function_name",NodeKinds::name}, {"Args definition",NodeKinds::function_head_args_definition}, {"ReturnType",NodeKinds::expression} });

	builder.addPolimorficNodeKind(NodeKinds::empty_expression,	"empty_expression",	NodeKinds::expression, {});
	builder.addPolimorficNodeKind(NodeKinds::function_call,	"function_call",	NodeKinds::expression, { { "Function",NodeKinds::expression},{"Parameters", NodeKinds::expression_list} });
	builder.addPolimorficNodeKind(NodeKinds::array_literal,	"array_literal",	NodeKinds::expression, { { "Elements", NodeKinds::expression_list } });
	builder.addPolimorficNodeKind(NodeKinds::Object_construct,	"Object_construct",	NodeKinds::expression, { { "Constructed Type",NodeKinds::expression},{"Parameters", NodeKinds::expression_list} });
	builder.addPolimorficNodeKind(NodeKinds::array_acess,		"array_acess",		NodeKinds::expression, { { "Array",NodeKinds::expression},{"indexes", NodeKinds::expression_list} });
	builder.addPolimorficNodeKind(NodeKinds::prefix_operator,	"prefix_operator",	NodeKinds::expression, { { "Operator",NodeKinds::leaf},{"right", NodeKinds::expression} });
	builder.addPolimorficNodeKind(NodeKinds::infix_operator,	"infix_operator",	NodeKinds::expression, { { "Operator",NodeKinds::leaf},{"left", NodeKinds::expression},{"right", NodeKinds::expression} });
	builder.addPolimorficNodeKind(NodeKinds::suffix_operator,	"suffix_operator",	NodeKinds::expression, { { "Operator",NodeKinds::leaf},{"left", NodeKinds::expression} });
	builder.addLeafToPolimorficNodeKind("variable", NodeKinds::expression);

	builder.addNodeKind(NodeKinds::statement,"statement", {});
	builder.addPolimorficNodeKind(NodeKinds::empty_stmt, "empty stmt",NodeKinds::statement, {});
	builder.addPolimorficNodeKind(NodeKinds::stmt_block, "block", NodeKinds::statement, {{"instruction",NodeKinds::statement}}, true);
	builder.addPolimorficNodeKind(NodeKinds::varible_declaration,"varible_declaration",NodeKinds::statement,{{"varible_name",0},{"varible_type",NodeKinds::expression}, {"varible_initial_value",NodeKinds::expression}});
	builder.addPolimorficNodeKind(NodeKinds::if_branch, "if", NodeKinds::statement, { {"cond",NodeKinds::expression},{"ifTrue",NodeKinds::statement}, {"ifFalse",NodeKinds::statement} });
	builder.addPolimorficNodeKind(NodeKinds::expression_stmt, "expression_stmt", NodeKinds::statement, { {"expr",NodeKinds::expression}});
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

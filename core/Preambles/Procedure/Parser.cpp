#include "Preambles/Procedure/Parser.h"

#include "Preambles/Procedure/Lexer.h"

// operatotrs on steatements for examle if expresion statement else statement 
// 
// statement := `if` bool_expresion statement [`else` statement]
// statement := `while` bool_expresion statement
// statement := `for` iterator_expresion statement
// statement := `do` statement `while` bool_expresion `;`
// statement := expresion `;`
// statement := `{` [statement..] `}`
// statement := `twice` statement
// 
// {} = blok kodu = statement with list of statements
// expresion = literal,operation, atom.atom is a operation on atoms
// expresion := expresion infix_op expresion
// expresion := prefix_op expresion
// expresion := expresion sufix_op
// expresion := `ifx` bool_expresion `then` expresion `else` expresion
// expresion := `send` message_expresion `to` agent_expresion
// expresion := `goto` label_atom
//
// require = if present consume otherwise panic
// optional = bool result and consume
// check = bool result and do not consume 
//


// foo(x : Int)->int

 using namespace Preamble::Procedure;


 void printError(const std::optional<ErrorT>& err) {
	 if (err.has_value()) {
		 std::cout << "ERROR : " << err->loc.start() << " : " << err->loc.val() << " = " << err.value().oneLinerError << std::endl;
	 }
 }

 void requireType(TokenStream& head) {
	 printError(head.require(Token::Type::atom));
 }

int parseHead(TokenStream& head){
	std::vector<ErrorT> errors;
	printError(head.require(Token::Type::atom));
	printError(head.require(Token::Type::parenthesis,"("));
	if(head.check(Token::Type::atom)){
		do {
			printError(head.require(Token::Type::atom));
			printError(head.require((Token::Type)ProcedureTokenType::colon));
			requireType(head);
		}while(head.optional((Token::Type)ProcedureTokenType::comma));
	}
	printError(head.require(Token::Type::parenthesis, ")"));
	if (head.optional((Token::Type)ProcedureTokenType::operator_t,"->")) {
		requireType(head);
	}
	printError(head.requireEmpty());
	return 0;
}
int parseBody(TokenStream& body){
	return 0;
}

IParser::TODODefineType Preamble::Procedure::Parser::parse(TokenStream& head, TokenStream& body)
{
	auto h = parseHead(head);
	auto b = parseBody(body);
}

Preamble::Procedure::Parser::~Parser()
{

}

// core.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include<format>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "CodeLocation.h"
#include "MetaLexer.h"

#include "Preambles/Procedure/Parser.h"
#include <Enumerate.h>

struct cliArgs {
	struct cliArg {
		uint8_t short_name;
		std::string longName;
		std::optional<std::string> default_value;
		std::optional<std::string> description;
	};
};

// if provided file dont contain build preamble use default build file
// if provided with "copy default build" create default build file insde curent dir 
// if provided with debug_build start interactive debuger and break right after ast is builded
// if provided with interactive compile project and start interactive shell before main
// 
// usage:
//  cerberus build.dog relasese_build # build using build.dog as build file and start function all
//  cerbersu build.dog # build using build.dog and all atom as entry if not founded but multiple build present print error with "i coude not decide with to choose"
//  cerbersu build.dog # build if only one build preamble present use it with warning if not named all
//  cerbersu build.dog # build if there is not any build preamble then use default build script
// 
//  cerberus --copy_default [<file_name>] # copy default build script into file location directory under name <file_name>, error if file exists # -F if to force
// 
//  cerberus --interactive_build <file_name> # start build and stop interactive on first build script
//  cerberus --interactive <file_name> # build produce 
//			# if --interactive defined and executed produce_exe or produce_elf give warning/info that you are sopouse to use 
//			# produce_exe_interactive() or produce_elf_interactive() # or produce_exe(,interactive=true) produce_elf(,interactive=true)
// 
// 

// stages : next stage only if no errors in preavius
//
// 1. lex input file
// 2. parse input file
// 3. find build preambule
// 4. intrprete/execute build function
//


int main(int argc, char** args)
{
	if (argc < 2) return -1;
	for (int i = 0; i < argc; i++) {
		std::cout << i << " : " << args[i] << std::endl;
	}
	PreambleRepository repo;
	//TODO you can only compile one file but file "compiled" is build script writen in cerberus that will call cerberus compiler again and again
	// useage : $ cerberus <build_filename>
	for (int file_count = 1; file_count < argc and std::string(args[file_count]) != "-o"; file_count++) {// FIXME -o makes output 
		MetaLexer lexer(repo, args[file_count]);
		uint64_t i = 0;
		std::vector<Token> head;
		bool addToHead = false;
		std::optional<Token> t = std::nullopt;
		std::vector<Token> body;
		bool addToBody = false;
		int paramCount = 0;
		bool skip = false;
		CodeLocation preamble = CodeLocation::null();
		while (auto a = lexer.lex()) {
			if (a->kind == Token::Type::comment) continue;
			if (skip == true and a->kind == Token::Type::preamble) skip = false;
			if (skip)std::cout << "Error Recovery: ";
			if (a->kind == Token::Type::preamble) std::cout << std::endl;
			if (a->kind == Token::Type::preamble) preamble = a->value;
			std::cout << std::format("\t\t\t({}/{} \"{}\") -> {}:{} Where = {}:{}:{} ", repo.prambleName(a.value()), repo.to_string(a.value()), a->value.to_string(), a->value.start(), a->value.end(), a->file, a->line, a->func) << std::endl;
			//std::cout <<"\t\t\t" << ++i << " : (" << repo.prambleName(a->kind) << "/" << repo.to_string(a->kind) << ") " << (a->value) << " -> " << a->value.start() << "-" << a->value.end() << std::endl;
			if (addToHead) {
				if (a.value().value == "{") {
					addToBody = true;
					addToHead = false;
				}
				else
					head.push_back(a.value());
			}
			if (a.value().value == "}" and paramCount == 0 and not addToBody) skip = true;
			if (addToBody) {
				body.push_back(a.value());

				if (a.value().value == "{") paramCount++;
				if (a.value().value == "}") paramCount--;

				if (paramCount == 0) {
					auto p = repo.get(repo.getPeambuleIndex(preamble))->parser;
					auto h = TokenStream(repo.getPeambuleIndex(preamble), head, repo);
					auto b = TokenStream(repo.getPeambuleIndex(preamble), body, repo);
					auto ast = p->parse(h, b);
					//std::cout << "Head : " << ast.headNode << std::endl;
					//std::cout << "Body : " << ast.bodyNode << std::endl;
					//for (const auto& [i, n] : enumerate(ast.nodes)) {
					//	std::cout << "[" << i << "] Kind(" << p->NodeKind_toString(n.kind) << ") ";
					//	for (const auto& j : n.children) {
					//		if (j.first == 0) std::cout << "Leaf(" << ast.leafs[j.second].value << ") ";
					//		else std::cout << j << " ";
					//	}
					//	std::cout << std::endl;
					//}
					addToBody = false;
					head.clear();
					body.clear();
					PreambleNode res{ std::map<Token, Token>{} ,t.value() ,ast};
					std::cout<< astToGraph(res,p);
				}
			}
			if (a->kind == Token::Type::preamble and a->value == "procedure") {
				addToHead = true;
				t = a;
			}
		}
	}

}

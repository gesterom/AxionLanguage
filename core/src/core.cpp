// core.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include<format>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "AST.h"
#include "CodeLocation.h"
#include "MetaLexer.h"
#include "MetaParser.h"
#include"PreambleDefinition.h"


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

//TODO remove function and type preamble 
// new idea
// preambile will work similar to rust macros but with better api 
// so implemented preamble will be build, type,procedure
// compiler intrinsic preamble
// `define operator` eg. ?.
// `define statement block` eg. keyword expression stmt
// 

// TODO remove TokenType system
// replace it with similar to NodeKind system where you need to register token type before you can use it.


int main(int argc, char** args)
{
	if (argc < 2) return -1;
	for (int i = 0; i < argc; i++) {
		std::cout << i << " : " << args[i] << std::endl;
	}
	SyntaxRepository repo;
	//TODO you can only compile one file but file "compiled" is build script writen in cerberus that will call cerberus compiler again and again
	// useage : $ cerberus <build_filename>
	for (int file_count = 1; file_count < argc and std::string(args[file_count]) != "-o"; file_count++) {// FIXME -o makes output 
		MetaLexer lexer(repo, args[file_count]);
		MetaParser parser(repo);
		// let a : optional(int&) = null;
		// A?Err -> Result<A,Err> 
		// int + int& + bool 
		// if a is int {
		// a -> int&
		// } else if a is int&{
		// a -> int&&
		// } else if a is bool or ERROR {
		// a-> bool&
		// }
		// 
		// if sum is int?int& {
		// 
		// }
		// 
		// option in compiler 
		// 
		// if Let Some(A) = x {
		//	A
		// }
		// if x is int = z { 
		// 
		// int maybe_a = optional
		// int a = a.value()
		// if a.has_value() {
		//  foo(a/*a is int*/)
		//	a = null;
		// }
		// let vec : vector(int) = vector{0};
		// if vec is int {
		//	
		// }
		// 
		// for let a in vec {
		//	
		// }
		// 
		//	return function (int)->string {a = 5; return "hey";};
		// } else {
		//	// a is null
		// }
		// 
		// 
		// 
		// if not a {
		//	// a is type int
		// } else {
		//	// a is null
		// }
		//
		while (auto preamble = parser.parseProgram(lexer)) {
			if (preamble->ast.headNode != std::nullopt and preamble->ast.bodyNode != std::nullopt)
				std::cout << astToGraph(preamble.value(),repo) << std::endl;
		}
	}

}

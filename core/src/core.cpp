// core.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iomanip>
#include <iostream>
#include <optional>
#include <string>

#include "MetaLexer.h"
#include "MetaParser.h"



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
//  wyazel build.w relasese_build # build using build.w as build file and start function all
//  wyazel build.w # build using build.w and `all` atom as entry if not founded but multiple build present print error with "i coude not decide witch to choose"
//  wyazel build.w # build if only one build preamble present use it with warning if not named all
//  wyazel build.w # build if there is not any build preamble then use default build script
// 
//  wyazel --copy_default [<file_name>] # copy default build script into file location directory under name <file_name>, error if file exists # -F if to force
// 
//  wyazel --interactive_build <file_name> # start build and stop interactive on first build script
//  wyazel --interactive <file_name> # build produce 
//			# if --interactive defined and executed produce_exe or produce_elf give warning/info that you are sopouse to use 
//			# produce_exe_interactive() or produce_elf_interactive() # or produce_exe(,interactive=true) produce_elf(,interactive=true)
// 
// 

// stages : next stage only if no errors in preavius
//
// 1-2 lex/parse input file
// 2. find build preambule
// 3. intrprete/execute build function
// 
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
	std::cout << "Commend line arguments:" << std::endl;
	for (int i = 0; i < argc; i++) {
		std::cout << "  Args[" << std::setw(3) << i << "] : " << args[i] << std::endl;
	}
	SyntaxRepository repo;
	//TODO you can only compile one file but file "compiled" is build script writen in Wyazel that will call Wyazel compiler again and again
	// useage : $ wyazel <build_filename>
	for (int file_count = 1; file_count < argc and std::string(args[file_count]) != "-o"; file_count++) {// FIXME -o makes output 
		MetaLexer lexer(repo, args[file_count]);
		MetaParser parser(repo);
		while (auto preamble = parser.parseProgram(lexer)) {
			if (preamble->ast.headNode != std::nullopt and preamble->ast.bodyNode != std::nullopt)
				std::cout << astToGraph(preamble.value(), repo) << std::endl;
		}
	}
}

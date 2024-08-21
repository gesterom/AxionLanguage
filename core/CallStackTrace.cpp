#include "CallStackTrace.h"


//#ifdef _WIN32
//#include <windows.h>
//#include <dbghelp.h>
#include <stacktrace>

//#pragma comment(lib, "dbghelp.lib")

std::vector<std::string> getCallStack() {
	std::vector<std::string> stack;
	#if _HAS_CXX23
	for (const auto& entry : std::stacktrace::current()) {
		
		stack.push_back(std::to_string(entry));
	}
	#endif
	return stack;
}

//#elif defined(__unix__) || defined(__APPLE__)
//#include <execinfo.h>
//#include <cxxabi.h>
//
//std::vector<std::string> getCallStack() {
//	std::vector<std::string> stack;
//	void* array[100];
//	size_t size;
//	char** symbols;
//
//	size = backtrace(array, 100);
//	symbols = backtrace_symbols(array, size);
//
//	for (size_t i = 0; i < size; i++) {
//		// Demangling the function names
//		char* demangled = abi::__cxa_demangle(symbols[i], NULL, NULL, NULL);
//		if (demangled) {
//			stack.push_back(std::string(demangled));
//			free(demangled);
//		}
//		else {
//			stack.push_back(std::string(symbols[i]));
//		}
//	}
//
//	free(symbols);
//	return stack;
//}
//
//#endif

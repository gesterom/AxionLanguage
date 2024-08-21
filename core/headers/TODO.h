#pragma once

#include <iostream>

#ifdef _WIN32
#define TODO(x) _TODO((__FILE__),(__LINE__),(__func__),(x))
#elif unix// __CYGWIN__
#define TODO(x) _TODO((__FILE__),(__LINE__),(__PREETY_FUNCTION__),(x))
#else
#define TODO(x) _TODO((__FILE__),(__LINE__),("<dontSuport_function_names>"),(x))
#endif

#ifdef _WIN32
#define ASSERT(x,y) do{ if(not (x)){ _assert((__FILE__),(__LINE__),(__func__),(#x),(x),(y)); } }while(false)
#elif unix// __CYGWIN__
#define ASSERT(x,y) do{ if(not (x)){ _assert((__FILE__),(__LINE__),(__PREETY_FUNCTION__),(#x),(x),(y)); } }while(false)
#else
#define ASSERT(x,y) do{ if(not (x)){ _assert((__FILE__),(__LINE__),("<dontSuport_function_names>"),(#x),(x),(y)); } }while(false)
#endif

#ifdef _WIN32
#define UNREACHABLE(x) _unreachable((__FILE__),(__LINE__),(__func__),(x))
#elif unix// __CYGWIN__
#define UNREACHABLE(x) _unreachable((__FILE__),(__LINE__),(__PREETY_FUNCTION__),(x))
#else
#define UNREACHABLE(x) _unreachable((__FILE__),(__LINE__),("<dontSuport_function_names>"),(x))
#endif

[[noreturn]]
void _TODO(std::string file, int64_t line, std::string function, const std::string& str);
[[noreturn]]
void _unreachable(std::string file, int64_t line, std::string function, const std::string& str);
void _assert(std::string file, int64_t line, std::string function, std::string cond_str, bool cond, const std::string& str);



#pragma comment(lib, "dbghelp.lib")
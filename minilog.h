#pragma onece

#include "print.h"
#include <source_location>
#include <format>
#include <iostream>
#include <cstdint>
#define FOREACH_LOG_LEVEL(f) \
f(trace)\
f(debug)\
f(info)\ 
f(critical)\
f(warning)\
f(error)\
f(fatal)
enum class log_level:uint8_t
{
   #define _FUNCTION(name) name,
   FOREACH_LOG_LEVEL(_FUNCTION)
   #undef _FUNCTION
};
inline std::string log_level_name(log_level lev)
{
   switch (lev)
   {
       #define _FUNCTION(name) case log_level::name:return #name;
       #undef _FUNCTION
   }
   return "unknow"; 

}
template<typename T>
struct with_source_location
{
   private:
   T inner;
   std::source_location loc;
   public:
   template<typename U> requires std::constructible_from<T,U>
   consteval with_source_location(U &&inner,std::source_location loc=std::source_location::current())
   :inner(std::forward<U>(inner)),loc(std::move(loc)){}
   constexpr T const &format() const {return inner;}
   constexpr std::source_location const &location() const {return loc;}

};

static log_level max_level=log_level::info;

template <typename ...Args>
void generic_log(log_level lev, with_source_location<std::format_string<Args...>> fmt,Args &&...args)
{  if(lev>=max_level)
   {
   auto const &loc=fmt.location();
   std::cout<<loc.file_name()<<":"<<loc.line()<<"["<<log_level_name(lev)<<"]"<<std::vformat(fmt.format().get(),std::make_format_args(args...))<<'\n';
   }
};

#define _FUNCTION(name)\
template <typename... Args>\
void log_##name(log_level lev, with_source_location<std::format_string<Args...>> fmt,Args &&...args)\
{\
   return generic_log(log_level::name,std::move(fmt),std::forward<Args>(args)...);\
}
FOREACH_LOG_LEVEL(_FUNCTION)
#undef __FUNCTON
#define LOG_P(x) log_debug(#x "={}",x)

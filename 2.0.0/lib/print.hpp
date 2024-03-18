#ifndef hfgen__print__hpp
#define hfgen__print__hpp

#ifndef __cplusplus
#error This header file is to only be used with C++ code
#endif /* __cplusplus */

/*
	Wrapper for outputting to C++ streams
	Made by anson in 2024, see LICENSE for related details
*/

#include "includes.hpp"

// These do not have an std::ostream object specified, so you will have to specify it
template<typename O> static O& PrintTo (O& o) { return o; }
template<typename O, typename T, typename... R>
static O& PrintTo(O& o, T&& t, R&&... r)
{
	return PrintTo((o << std::forward<T>(t), o), std::forward<R>(r)...);
}

// Meanwhile these direct to std::cout
template<typename... T> static inline void Print  (T&&... t) { PrintTo(std::cout, std::forward<T&&>(t)...); }
template<typename... T> static inline void Println(T&&... t) { PrintTo(std::cout, std::forward<T&&>(t)..., "\n"); }

#endif /* hfgen__print__hpp */

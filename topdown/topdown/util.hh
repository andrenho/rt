#ifndef TOPDOWN_UTIL_HH
#define TOPDOWN_UTIL_HH

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif //TOPDOWN_UTIL_HH

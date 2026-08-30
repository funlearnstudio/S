#define S_PLATFORM_IMPL
#include "platform.cpp"

// platform.cpp and ecosystem.cpp are intentionally compiled into the same
// translation unit so `se build` gets the same builtin runtime without a
// second native-link path. Rename ecosystem-local helper functions while
// including it so their anonymous-namespace details cannot collide. These are
// function-like macros on purpose: a field such as TypeInfo::callable stays
// untouched because it is not followed by `(`.
#define callable(...) eco_callable(__VA_ARGS__)
#define module_type(...) eco_module_type(__VA_ARGS__)
#define list_type(...) eco_list_type(__VA_ARGS__)
#define make_module(...) eco_make_module(__VA_ARGS__)
#define shell_quote(...) eco_shell_quote(__VA_ARGS__)
#define normalized_system(...) eco_normalized_system(__VA_ARGS__)
#define process_output(...) eco_process_output(__VA_ARGS__)
#include "ecosystem.cpp"
#undef process_output
#undef normalized_system
#undef shell_quote
#undef make_module
#undef list_type
#undef module_type
#undef callable

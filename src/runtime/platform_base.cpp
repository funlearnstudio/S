#define S_PLATFORM_IMPL
#include "platform.cpp"

// platform.cpp and ecosystem.cpp are intentionally compiled into the same
// translation unit so `se build` gets the same builtin runtime without a
// second native-link path. Rename ecosystem-local helpers while including it
// so their anonymous-namespace implementation details cannot collide.
#define callable eco_callable
#define module_type eco_module_type
#define list_type eco_list_type
#define make_module eco_make_module
#define shell_quote eco_shell_quote
#define normalized_system eco_normalized_system
#define process_output eco_process_output
#include "ecosystem.cpp"
#undef process_output
#undef normalized_system
#undef shell_quote
#undef make_module
#undef list_type
#undef module_type
#undef callable

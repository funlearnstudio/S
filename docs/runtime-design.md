# Runtime Design

S values use a closed C++ variant: None, Int, Num, Bool, Text, List, and Function. Lists and functions use managed shared ownership; S code never sees raw pointers, `new`, `delete`, `malloc`, or `free`.

Runtime operations validate their inputs. Division by zero, invalid members, wrong function arity, non-Bool conditions, and out-of-range indexes become S errors rather than undefined behavior.

The interpreter uses lexical environments and function closures. The native backend emits equivalent checked helpers into each standalone executable. Future runtime modules will add files and media behind RAII handles.

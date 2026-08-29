cmake_minimum_required(VERSION 3.20)
project(S VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(s_core
  src/lexer/token.cpp src/lexer/lexer.cpp src/parser/parser.cpp
  src/runtime/error.cpp src/runtime/value.cpp src/interpreter/interpreter.cpp
  src/type/checker.cpp src/compiler/compiler.cpp)
target_include_directories(s_core PUBLIC include)
if(MSVC)
  target_compile_options(s_core PRIVATE /W4 /permissive-)
else()
  target_compile_options(s_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(s src/main.cpp)
target_link_libraries(s PRIVATE s_core)

include(CTest)
if(BUILD_TESTING)
  add_executable(s_tests tests/tests.cpp)
  target_link_libraries(s_tests PRIVATE s_core)
  add_test(NAME s_tests COMMAND s_tests)
  add_test(NAME cli_check COMMAND s check ${CMAKE_SOURCE_DIR}/examples/hello.s)
  add_test(NAME cli_run COMMAND s run ${CMAKE_SOURCE_DIR}/examples/function.s)
endif()

install(TARGETS s RUNTIME DESTINATION bin)

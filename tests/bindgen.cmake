file(REMOVE_RECURSE "${S_WORK}")
file(MAKE_DIRECTORY "${S_WORK}")

execute_process(
  COMMAND "${S_EXE}" bind "${S_ROOT}/native/native_test.sbind" "${S_WORK}"
  RESULT_VARIABLE bind_code
  OUTPUT_VARIABLE bind_out
  ERROR_VARIABLE bind_err)
if(NOT bind_code EQUAL 0)
  message(FATAL_ERROR "s bind failed:\n${bind_out}\n${bind_err}")
endif()

set(meta "${S_WORK}/generated_native_test.snative")
set(header "${S_WORK}/generated_native_test_bindings.h")
set(source "${S_WORK}/generated_native_test_bindings.cpp")
foreach(path IN ITEMS "${meta}" "${header}" "${source}")
  if(NOT EXISTS "${path}")
    message(FATAL_ERROR "s bind did not generate ${path}")
  endif()
endforeach()

file(READ "${meta}" meta_text)
if(NOT meta_text MATCHES "bytes_echo s_generated_native_test_bytes_echo Bytes -> Bytes")
  message(FATAL_ERROR "Generated metadata does not contain the Bytes binding.")
endif()

set(library "${S_WORK}/libgenerated_native_test.so")
execute_process(
  COMMAND "${S_CXX}" -std=c++20 -shared -fPIC -Wall -Wextra -Wpedantic -Werror
          "-I${S_ROOT}/include" "-I${S_ROOT}/native" "-I${S_WORK}"
          "${source}" "${S_ROOT}/native/native_test.c" -o "${library}"
  RESULT_VARIABLE compile_code
  OUTPUT_VARIABLE compile_out
  ERROR_VARIABLE compile_err)
if(NOT compile_code EQUAL 0)
  message(FATAL_ERROR "Generated wrapper did not compile:\n${compile_out}\n${compile_err}")
endif()

set(program "${S_WORK}/generated_test.s")
file(WRITE "${program}" [=[use generated_native_test

say add 2 3
say square 3.0
say not_bool false
say echo "generated"
data = bytes "abcde"
say bytes_len data
copy = bytes_echo data
say copy.len
try
    value = native_error 1
    say value
else err
    say err.kind
]=])

set(ENV{S_NATIVE_PATH} "${S_WORK}")
execute_process(COMMAND "${S_EXE}" check "${program}" RESULT_VARIABLE check_code OUTPUT_VARIABLE check_out ERROR_VARIABLE check_err)
if(NOT check_code EQUAL 0)
  message(FATAL_ERROR "Generated module failed s check:\n${check_out}\n${check_err}")
endif()

execute_process(COMMAND "${S_EXE}" run "${program}" RESULT_VARIABLE run_code OUTPUT_VARIABLE run_out ERROR_VARIABLE run_err)
if(NOT run_code EQUAL 0)
  message(FATAL_ERROR "Generated module failed s run:\n${run_err}")
endif()

execute_process(COMMAND "${S_EXE}" build "${program}" RESULT_VARIABLE build_code OUTPUT_VARIABLE build_out ERROR_VARIABLE build_err)
if(NOT build_code EQUAL 0)
  message(FATAL_ERROR "Generated module failed s build:\n${build_out}\n${build_err}")
endif()

execute_process(COMMAND "${S_WORK}/generated_test" RESULT_VARIABLE native_code OUTPUT_VARIABLE native_out ERROR_VARIABLE native_err)
if(NOT native_code EQUAL 0)
  message(FATAL_ERROR "Generated native executable failed:\n${native_err}")
endif()

if(NOT run_out STREQUAL native_out)
  message(FATAL_ERROR "Generated binding Interpreter/native mismatch.\nrun: ${run_out}\nnative: ${native_out}")
endif()

set(expected "5\n9\ntrue\ngenerated\n5\n5\nNativeError\n")
if(NOT run_out STREQUAL expected)
  message(FATAL_ERROR "Generated binding output mismatch.\nexpected: ${expected}\nactual: ${run_out}")
endif()

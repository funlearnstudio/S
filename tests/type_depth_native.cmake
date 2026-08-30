if(NOT DEFINED SE_EXE OR NOT DEFINED SE_ROOT OR NOT DEFINED SE_WORK)
  message(FATAL_ERROR "SE_EXE, SE_ROOT and SE_WORK are required")
endif()

file(REMOVE_RECURSE "${SE_WORK}")
file(MAKE_DIRECTORY "${SE_WORK}")
file(COPY "${SE_ROOT}/examples/type-depth.se" DESTINATION "${SE_WORK}")

execute_process(
  COMMAND "${SE_EXE}" build "${SE_WORK}/type-depth.se"
  WORKING_DIRECTORY "${SE_WORK}"
  RESULT_VARIABLE build_result
  OUTPUT_VARIABLE build_out
  ERROR_VARIABLE build_err)
if(NOT build_result EQUAL 0)
  message(FATAL_ERROR "SE type-depth native build failed:\n${build_out}\n${build_err}")
endif()

execute_process(
  COMMAND "${SE_WORK}/type-depth"
  WORKING_DIRECTORY "${SE_WORK}"
  RESULT_VARIABLE run_result
  OUTPUT_VARIABLE run_out
  ERROR_VARIABLE run_err)
if(NOT run_result EQUAL 0)
  message(FATAL_ERROR "SE type-depth native executable failed:\n${run_out}\n${run_err}")
endif()

if(NOT run_out STREQUAL "10\nSE\n7\n")
  message(FATAL_ERROR "SE type-depth native output mismatch:\n${run_out}")
endif()

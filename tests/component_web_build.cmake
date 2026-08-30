file(REMOVE_RECURSE "${SE_WORK}")
file(MAKE_DIRECTORY "${SE_WORK}")
execute_process(
  COMMAND "${SE_EXE}" web build "${SE_ROOT}/examples/component-web.se" "${SE_WORK}/dist"
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE error
)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "SE component web build failed:\n${output}\n${error}")
endif()
foreach(name index.html style.css app.js app.ts)
  if(NOT EXISTS "${SE_WORK}/dist/${name}")
    message(FATAL_ERROR "Missing generated ${name}")
  endif()
endforeach()
file(READ "${SE_WORK}/dist/index.html" html)
file(READ "${SE_WORK}/dist/style.css" css)
file(READ "${SE_WORK}/dist/app.js" js)
string(REGEX MATCHALL "data-se-instance=\"se-Button-[0-9]+\"" button_instances "${html}")
list(LENGTH button_instances button_count)
if(NOT button_count EQUAL 3)
  message(FATAL_ERROR "Expected three independent Button instances, got ${button_count}\n${html}")
endif()
if(NOT html MATCHES ">Save</button>" OR NOT html MATCHES ">Cancel</button>" OR NOT html MATCHES ">Next</button>")
  message(FATAL_ERROR "Generated HTML is missing Button text\n${html}")
endif()
if(NOT css MATCHES "\\.se-Button" OR NOT css MATCHES "padding: 12px" OR NOT css MATCHES "border-radius: 8px")
  message(FATAL_ERROR "Generated component CSS is missing scoped Button styles\n${css}")
endif()
if(NOT css MATCHES "\\.se-Button:hover" OR NOT css MATCHES "opacity: 0.8")
  message(FATAL_ERROR "Generated component CSS is missing scoped nested selector\n${css}")
endif()
if(NOT js MATCHES "addEventListener\\(\"click\"" OR NOT js MATCHES "Save" OR NOT js MATCHES "Cancel" OR NOT js MATCHES "Next")
  message(FATAL_ERROR "Generated JavaScript is missing instance event bindings\n${js}")
endif()
if(NOT js MATCHES "const browser = Object.freeze" OR NOT js MATCHES "async event =>")
  message(FATAL_ERROR "Generated JavaScript is missing the SE browser runtime or async event lowering\n${js}")
endif()

execute_process(
  COMMAND "${SE_EXE}" web build "${SE_ROOT}/examples/browser-api.se" "${SE_WORK}/browser"
  RESULT_VARIABLE browser_result
  OUTPUT_VARIABLE browser_output
  ERROR_VARIABLE browser_error
)
if(NOT browser_result EQUAL 0)
  message(FATAL_ERROR "SE browser API example failed to build:\n${browser_output}\n${browser_error}")
endif()
file(READ "${SE_WORK}/browser/index.html" browser_html)
file(READ "${SE_WORK}/browser/app.js" browser_js)
if(NOT browser_html MATCHES "data-se-page=\"/\"" OR NOT browser_html MATCHES "data-se-page=\"/api\"" OR NOT browser_html MATCHES "data-se-page=\"/send\"" OR NOT browser_html MATCHES "data-se-page=\"/about\"")
  message(FATAL_ERROR "Generated browser example is missing multi-page route containers\n${browser_html}")
endif()
if(NOT browser_html MATCHES "id=\"message-form\"" OR NOT browser_html MATCHES "id=\"api-output\"")
  message(FATAL_ERROR "Generated browser example is missing HTML attributes/content lowering\n${browser_html}")
endif()
if(NOT browser_js MATCHES "browser.get_json" OR NOT browser_js MATCHES "browser.post_json" OR NOT browser_js MATCHES "await \\(task\\)" OR NOT browser_js MATCHES "browser.go")
  message(FATAL_ERROR "Generated browser example is missing API, await, sending, or navigation lowering\n${browser_js}")
endif()
if(NOT browser_js MATCHES "AbortController" OR NOT browser_js MATCHES "Retry-After" AND NOT browser_js MATCHES "retry-after" OR NOT browser_js MATCHES "Network request failed")
  message(FATAL_ERROR "Generated browser runtime is missing cancellation/retry/network handling\n${browser_js}")
endif()

execute_process(
  COMMAND "${SE_EXE}" web build "${SE_ROOT}/examples/component-web-invalid.se" "${SE_WORK}/invalid"
  RESULT_VARIABLE invalid_result
  OUTPUT_VARIABLE invalid_output
  ERROR_VARIABLE invalid_error
)
if(invalid_result EQUAL 0)
  message(FATAL_ERROR "Invalid component arity unexpectedly built successfully")
endif()
if(NOT invalid_error MATCHES "Component 'Button' needs 1 value\\(s\\), but got 2")
  message(FATAL_ERROR "Unexpected component diagnostic:\n${invalid_output}\n${invalid_error}")
endif()
message(STATUS "SE component web, browser API, routing and diagnostics passed")

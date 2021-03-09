# Based off https://github.com/lefticus/cpp_starter_project/blob/master/cmake/Sanitizers.cmake
# Licence : The Unlicense
# Adapted for C and stripped for the scale of this project

function(enable_sanitizers project_name)

  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)

    if(ENABLE_COVERAGE)
      target_compile_options(${project_name} INTERFACE --coverage -O0 -g)
      target_link_libraries(${project_name} INTERFACE --coverage)
    endif()

    set(SANITIZERS "")

    option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" FALSE)
    if(ENABLE_SANITIZER_ADDRESS)
      list(APPEND SANITIZERS "address")
    endif()

    if(!APPLE)
      option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" FALSE)
      if(ENABLE_SANITIZER_LEAK)
        list(APPEND SANITIZERS "leak")
      endif()
    endif()
    
    option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" FALSE)
    if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
      list(APPEND SANITIZERS "undefined")
    endif()

    if(!APPLE)
      option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" FALSE)
      if(ENABLE_SANITIZER_MEMORY AND CMAKE_C_COMPILER_ID MATCHES ".*Clang")
        if("address" IN_LIST SANITIZERS
          OR "leak" IN_LIST SANITIZERS)
          message(WARNING "Memory sanitizer does not work with Address and Leak sanitizer enabled")
        else()
          list(APPEND SANITIZERS "memory")
        endif()
      endif()
    endif()

    list(
      JOIN
      SANITIZERS
      ","
      LIST_OF_SANITIZERS)

  endif()

  if(LIST_OF_SANITIZERS)
    if(NOT
       "${LIST_OF_SANITIZERS}"
       STREQUAL
       "")
      target_compile_options(${project_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
      target_link_options(${project_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
    endif()
  endif()

endfunction()

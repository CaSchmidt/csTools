function(format_output_name target name)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Invalid <target>!")
  endif()

  if(NOT name)
    message(FATAL_ERROR "Invalid <name>!")
  endif()

  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    string(APPEND name "-x86")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    string(APPEND name "-x64")
  endif()

  get_target_property(type ${target} TYPE)
  if(type STREQUAL "STATIC_LIBRARY")
    string(APPEND name "-static")
  endif()

  get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(NOT is_multi_config)
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
      string(APPEND name "-${CMAKE_BUILD_TYPE}")
    endif()
    set_target_properties(${target} PROPERTIES OUTPUT_NAME ${name})
  else()
    set_target_properties(${target} PROPERTIES OUTPUT_NAME "${name}$<$<NOT:$<CONFIG:Release>>:-$<CONFIG>>")
  endif()
endfunction(format_output_name)

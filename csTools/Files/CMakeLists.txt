### Dependencies #############################################################

set(PCRE2_BUILD_PCRE2_8   ON  CACHE BOOL "" FORCE)
set(PCRE2_DEBUG           OFF CACHE BOOL "" FORCE)
set(PCRE2_SUPPORT_JIT     ON  CACHE BOOL "" FORCE)
set(PCRE2_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)

add_subdirectory(${csTools_SOURCE_DIR}/../3rdparty/pcre2-10.35
  ${csTools_BINARY_DIR}/3rdparty/pcre2
)

### Subdirectories ###########################################################

add_subdirectory(find)
add_subdirectory(matching)
add_subdirectory(ui)

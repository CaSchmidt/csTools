### Project ##################################################################

list(APPEND calculate_HEADERS
  include/Calculate/Parser.h
)

list(APPEND calculate_SOURCES
  src/Parser.cpp
)

### Target ###################################################################

add_library(calculate STATIC)

format_output_name(calculate "calculate")

target_include_directories(calculate
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(calculate PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_sources(calculate
  PRIVATE ${calculate_HEADERS}
  PRIVATE ${calculate_SOURCES}
)

target_link_libraries(calculate PUBLIC csUtil)

### Target CLI ###############################################################

add_executable(calculatecli
  src/main_calculatecli.cpp
)

format_output_name(calculatecli "calculatecli")

target_include_directories(calculatecli
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(calculatecli PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_link_libraries(calculatecli
  PRIVATE calculate
)

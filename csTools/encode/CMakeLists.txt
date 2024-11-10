### Project ##################################################################

list(APPEND encode_HEADERS
  include/Encode/Engine.h
  include/Encode/Field.h
  include/Encode/List.h
  include/Encode/Literal.h
  include/Encode/Parser.h
  include/Encode/Store.h
  include/Encode/Variable.h
)

list(APPEND encode_SOURCES
  src/Parser.cpp
)

### Target ###################################################################

add_library(encode STATIC)

format_output_name(encode "encode")

target_include_directories(encode
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(encode PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_sources(encode
  PRIVATE ${encode_HEADERS}
  PRIVATE ${encode_SOURCES}
)

target_link_libraries(encode PUBLIC csUtil)

### Target CLI ###############################################################

add_executable(encodecli
  src/main_encodecli.cpp
)

format_output_name(encodecli "encodecli")

target_include_directories(encodecli
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(encodecli PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_link_libraries(encodecli
  PRIVATE encode
)

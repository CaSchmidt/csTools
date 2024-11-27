### Project ##################################################################

list(APPEND log2pcap_HEADERS
  include/PCAP.h
  include/SocketCAN.h
)

list(APPEND log2pcap_SOURCES
  src/main_log2pcap.cpp
)

### Test Data ################################################################

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples/candump-2024-09-01_173152.log
  DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testdata
)

### Target ###################################################################

add_executable(log2pcap)

format_output_name(log2pcap "log2pcap")

set_target_properties(log2pcap PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

target_include_directories(log2pcap
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(log2pcap
  PRIVATE ${log2pcap_HEADERS}
  PRIVATE ${log2pcap_SOURCES}
)

target_link_libraries(log2pcap
  PRIVATE csUtil
)

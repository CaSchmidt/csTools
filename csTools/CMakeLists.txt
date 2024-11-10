cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

### Project ##################################################################

project(csTools)

# Custom CMake utilities
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FormatOutputName)

### Dependencies #############################################################

set(ENABLE_QT       ON CACHE BOOL "" FORCE)
set(ENABLE_QTIMPORT ON CACHE BOOL "" FORCE)

add_subdirectory(${csTools_SOURCE_DIR}/../3rdparty/csUtil/csUtil
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/csUtil
)

if(ENABLE_QT)
  find_package(Qt5Widgets 5.12 REQUIRED)
  find_package(Qt5Xml 5.12 REQUIRED)
endif()

### Tools ####################################################################

add_subdirectory(calculate)
add_subdirectory(encode)
add_subdirectory(log2pcap)

if(ENABLE_QT AND ENABLE_QTIMPORT)
  add_subdirectory(BinTools)
endif()

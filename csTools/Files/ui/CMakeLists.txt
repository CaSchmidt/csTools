### Project ##################################################################

list(APPEND Files_FORMS
  forms/WFind.ui
  forms/WGrep.ui
  forms/WMainWindow.ui
)

list(APPEND Files_HEADERS
  include/FilesModel.h
  include/ITabWidget.h
  include/MatchJob.h
  include/MatchResultsModel.h
  include/ResultsProxyDelegate.h
  include/Settings.h
  include/WFileList.h
  include/WFind.h
  include/WGrep.h
  include/WMainWindow.h
)

list(APPEND Files_SOURCES
  src/FilesModel.cpp
  src/ITabWidget.cpp
  src/MatchJob.cpp
  src/MatchResultsModel.cpp
  src/ResultsproxyDelegate.cpp
  src/Settings.cpp
  src/WFileList.cpp
  src/WFind.cpp
  src/WGrep.cpp
  src/WMainWindow.cpp
  src/main.cpp
)

### Target ###################################################################

add_executable(Files
  ${Files_FORMS}
  ${Files_HEADERS}
  ${Files_SOURCES}
)

format_output_name(Files "csFiles")

set_target_properties(Files PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
  AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
)

if(WIN32)
  set_target_properties(Files PROPERTIES
    WIN32_EXECUTABLE ON
  )
endif()

target_compile_definitions(Files
  PRIVATE -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII
)

target_include_directories(Files PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Files find matching csUtil Qt5::Concurrent Qt5::Widgets)

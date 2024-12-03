### Project ##################################################################

list(APPEND BinTools_FILES
  resources/resources.qrc
)

list(APPEND BinTools_FORMS
  forms/WCalculatorPage.ui
  forms/WEncoderPage.ui
  forms/WMainWindow.ui
)

list(APPEND BinTools_HEADERS
  include/Calculator/WCalculatorPage.h
  include/Encoder/EncodeResultsModel.h
  include/Encoder/EncodeVariablesModel.h
  include/Encoder/WEncoderPage.h
  include/global.h
  include/WMainWindow.h
  include/WTabPageBase.h
  include/XML_io.h
  include/XML_tags.h
)

list(APPEND BinTools_SOURCES
  src/Calculator/WCalculatorPage.cpp
  src/Encoder/EncodeResultsModel.cpp
  src/Encoder/EncodeVariablesModel.cpp
  src/Encoder/WEncoderPage.cpp
  src/global.cpp
  src/WMainWindow.cpp
  src/WTabPageBase.cpp
  src/XML_io.cpp
  src/main.cpp
)

### Target ###################################################################

add_executable(BinTools)

format_output_name(BinTools "csBinTools")

target_compile_definitions(BinTools
  PRIVATE QT_NO_CAST_FROM_ASCII
  PRIVATE QT_NO_CAST_TO_ASCII
)

target_include_directories(BinTools
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(BinTools
  PRIVATE ${BinTools_FILES}
  PRIVATE ${BinTools_FORMS}
  PRIVATE ${BinTools_HEADERS}
  PRIVATE ${BinTools_SOURCES}
)

set_target_properties(BinTools PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

set_target_properties(BinTools PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
)

set_property(TARGET BinTools APPEND PROPERTY
  AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
)

if(WIN32)
  set_target_properties(BinTools PROPERTIES
    WIN32_EXECUTABLE ON
  )
endif()

target_link_libraries(BinTools
  PRIVATE encode
)

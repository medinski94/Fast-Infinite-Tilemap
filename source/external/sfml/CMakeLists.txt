message(STATUS "Fetching SFML...")

FetchContent_Declare(
  sfml
  URL https://github.com/SFML/SFML/archive/refs/tags/2.5.1.zip
)

set(SFML_BUILD_AUDIO false)
set(SFML_BUILD_NETWORK false)
set(SFML_BUILD_EXAMPLES false)
set(SFML_BUILD_DOC false)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
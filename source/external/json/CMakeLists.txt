message(STATUS "Fetching JSON...")

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/archive/refs/tags/v3.10.5.zip
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

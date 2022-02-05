message(STATUS "Fetching ImGui...")

FetchContent_Declare(
  imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.86.zip
)

FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  imgui-sfml
  URL https://github.com/eliasdaler/imgui-sfml/archive/refs/tags/v2.5.zip
)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML false)
set(IMGUI_SFML_IMGUI_DEMO false)

FetchContent_GetProperties(imgui-sfml)
if(NOT imgui-sfml_POPULATED)
    FetchContent_Populate(imgui-sfml)
    add_subdirectory(${imgui-sfml_SOURCE_DIR} ${imgui-sfml_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set_target_properties(ImGui-SFML PROPERTIES PUBLIC_HEADER "")

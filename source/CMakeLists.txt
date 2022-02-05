add_subdirectory(${SOURCE_DIR}/external)

add_executable(${EXECUTABLE_NAME}
    ${SOURCE_DIR}/src/main.cpp
	${SOURCE_DIR}/src/Engine.cpp
	${SOURCE_DIR}/src/Game.cpp
	${SOURCE_DIR}/src/PageManager.cpp
	${SOURCE_DIR}/src/World.cpp
	${SOURCE_DIR}/src/Camera.cpp
	${SOURCE_DIR}/src/UI.cpp
	${SOURCE_DIR}/src/DebugInfo.cpp
    ${SOURCE_DIR}/src/Tilemap.cpp
	${SOURCE_DIR}/src/Chunk.cpp
	${SOURCE_DIR}/src/TileBrush.cpp
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${SOURCE_DIR}/include)

target_link_libraries(${EXECUTABLE_NAME}
    ImGui-SFML::ImGui-SFML 
    fastnoise-simd
    nlohmann_json::nlohmann_json
)

if(WIN32)
    if (BUILD_SHARED_LIBS)
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
                $<TARGET_FILE:sfml-graphics>
                $<TARGET_FILE:sfml-window>
                $<TARGET_FILE:sfml-system>
                $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>)
    endif()
endif()

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/assets)

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION .)
install(DIRECTORY ${SOURCE_DIR}/assets DESTINATION .)

if (BUILD_SHARED_LIBS)
    install(TARGETS ImGui-SFML sfml-graphics sfml-window sfml-system RUNTIME DESTINATION .)
endif()

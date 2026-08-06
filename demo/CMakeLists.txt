# Copy Asteroid App and Plugin
add_custom_target(asteroid_app ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/asteroid
        ${CMAKE_BINARY_DIR}/bin/asteroid
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_BINARY_DIR}/bin/asteroid/plugins
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:spectre_plugin>
        ${CMAKE_BINARY_DIR}/bin/asteroid/plugins/
    DEPENDS spectre_plugin
)

# Copy Editor App and Plugin
add_custom_target(editor_app ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/editor
        ${CMAKE_BINARY_DIR}/bin/editor
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_BINARY_DIR}/bin/editor/plugins
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:spectre_plugin>
        ${CMAKE_BINARY_DIR}/bin/editor/plugins/
    DEPENDS spectre_plugin
)

# Ensure building the launcher also triggers copying the apps and plugins
if (TARGET sandbox_launcher)
    add_dependencies(sandbox_launcher asteroid_app editor_app)
endif()
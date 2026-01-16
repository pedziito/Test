cmake_minimum_required(VERSION 3.10)
project(PluginSystem VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

# Main application
add_executable(plugin_app
    main.cpp
    plugin_manager.cpp
)

target_include_directories(plugin_app PRIVATE ${CMAKE_SOURCE_DIR})

# Link with dl library on Unix systems for dynamic loading
if(UNIX)
    target_link_libraries(plugin_app PRIVATE ${CMAKE_DL_LIBS})
endif()

# Hello Plugin
add_library(hello_plugin SHARED
    plugins/hello_plugin.cpp
)
target_include_directories(hello_plugin PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(hello_plugin PROPERTIES PREFIX "")

# Math Plugin
add_library(math_plugin SHARED
    plugins/math_plugin.cpp
)
target_include_directories(math_plugin PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(math_plugin PROPERTIES PREFIX "")

# Text Plugin
add_library(text_plugin SHARED
    plugins/text_plugin.cpp
)
target_include_directories(text_plugin PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(text_plugin PROPERTIES PREFIX "")

# Custom target to run the application
add_custom_target(run
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugin_app ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    DEPENDS plugin_app hello_plugin math_plugin text_plugin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running plugin application..."
)

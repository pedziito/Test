cmake_minimum_required(VERSION 3.15)
project(PluginInjectorSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(CURL REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Debugger library (shared code)
add_library(debugger STATIC
    src/debugger.cpp
)
target_include_directories(debugger PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)
target_link_libraries(debugger PUBLIC ${CURL_LIBRARIES})
set_target_properties(debugger PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Plugin Injector executable
add_executable(plugin_injector
    src/plugin_injector.cpp
)
target_include_directories(plugin_injector PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Plugin Loader executable
add_executable(plugin_loader
    src/plugin_loader.cpp
)
target_include_directories(plugin_loader PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(plugin_loader ${CMAKE_DL_LIBS})

# Web Server executable (GUI interface)
add_executable(web_server
    src/web_server.cpp
)
target_include_directories(web_server PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(web_server pthread)

# Function to create plugin targets
function(add_plugin PLUGIN_NAME SOURCE_FILE)
    add_library(${PLUGIN_NAME} SHARED ${SOURCE_FILE})
    target_include_directories(${PLUGIN_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(${PLUGIN_NAME} debugger)
    set_target_properties(${PLUGIN_NAME} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins_injected
    )
endfunction()

# Build plugins from injected source files
file(GLOB INJECTED_PLUGINS ${CMAKE_SOURCE_DIR}/plugins_injected/*.cpp)
foreach(PLUGIN_SOURCE ${INJECTED_PLUGINS})
    get_filename_component(PLUGIN_NAME ${PLUGIN_SOURCE} NAME_WE)
    add_plugin(${PLUGIN_NAME} ${PLUGIN_SOURCE})
    message(STATUS "Added injected plugin: ${PLUGIN_NAME}")
endforeach()

# Custom target to run the injector before building plugins
add_custom_target(inject_plugins
    COMMAND ${CMAKE_BINARY_DIR}/plugin_injector ${CMAKE_SOURCE_DIR}/plugins ${CMAKE_SOURCE_DIR}/plugins_injected
    DEPENDS plugin_injector
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Injecting debugger code into plugins..."
)

# Print build information
message(STATUS "")
message(STATUS "=== Plugin Injector System Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
message(STATUS "")

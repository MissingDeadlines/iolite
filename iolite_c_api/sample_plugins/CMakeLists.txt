cmake_minimum_required(VERSION 3.26)

project(IOLITE_API_SAMPLES)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_C_STANDARD 90)

if(WIN32)
  set(OUTPUT_DIR "windows")
elseif(UNIX)
  set(OUTPUT_DIR "linux")
endif()

include_directories(../ dependencies/imgui dependencies/glm/glm dependencies/)
add_definitions("-D_CRT_SECURE_NO_WARNINGS")

add_library(IoliteSamplePluginC SHARED sample_plugin.c)
add_library(IoliteSamplePluginVulkan SHARED sample_plugin_vulkan.c)

add_library(IoliteSamplePluginCPP SHARED
  sample_plugin.cpp
  dependencies/imgui/imgui.cpp
  dependencies/imgui/imgui_widgets.cpp
  dependencies/imgui/imgui_tables.cpp
  dependencies/imgui/imgui_draw.cpp
)
add_library(IoliteMinimalPlugin SHARED minimal_plugin.cpp)

foreach(EXECUTABLE IoliteSamplePluginC IoliteSamplePluginCPP IoliteMinimalPlugin IoliteSamplePluginVulkan)
  set_target_properties(${EXECUTABLE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/${OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/${OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/${OUTPUT_DIR}
    RUNTIME_OUTPUT_NAME ${EXECUTABLE}
    RUNTIME_OUTPUT_NAME_RELEASE ${EXECUTABLE}
    RUNTIME_OUTPUT_NAME_RELWITHDEBINFO ${EXECUTABLE}
    RUNTIME_OUTPUT_NAME_DEBUG ${EXECUTABLE}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${OUTPUT_DIR}
    LIBRARY_OUTPUT_NAME ${EXECUTABLE}
  )
endforeach()
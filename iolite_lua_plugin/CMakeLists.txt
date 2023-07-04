cmake_minimum_required(VERSION 3.26)

project(IOLITE_LUA_PLUGIN)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LuaJIT
add_definitions("-DSOL_LUAJIT=1")
add_definitions("-DSOL_ALL_SAFETIES_ON=1")

if(WIN32)
  find_library(LUA_JIT_LIBRARIES NAMES lua51 PATHS "dependencies/lua/lib")
elseif(UNIX)
  find_library(LUA_JIT_LIBRARIES NAMES luajit PATHS "dependencies/lua/lib")
endif()

include_directories(../iolite_c_api/sample_plugins/dependencies/glm/glm ../iolite_c_api dependencies dependencies/lua/include ./)

add_library(IoliteLuaPlugin SHARED
  lua_plugin.cpp
)

target_link_libraries(IoliteLuaPlugin ${LUA_JIT_LIBRARIES})

foreach(EXECUTABLE IoliteLuaPlugin)
  set_target_properties(${EXECUTABLE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_NAME ${EXECUTABLE}
    RUNTIME_OUTPUT_NAME_RELEASE ${EXECUTABLE}
    RUNTIME_OUTPUT_NAME_RELWITHDEBINFO ${EXECUTABLE}
    RUNTIME_OUTPUT_NAME_DEBUG ${EXECUTABLE}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    LIBRARY_OUTPUT_NAME ${EXECUTABLE}
  )
endforeach()
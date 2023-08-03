cmake_minimum_required(VERSION 3.26)

project(IOLITE_PLUGINS)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(IO_PLUGIN_ID)
  add_definitions("-DIO_PLUGIN_ID=${IO_PLUGIN_ID}")
else()
  remove_definitions(-DIO_PLUGIN_ID)
endif()

unset(IO_PLUGIN_ID CACHE)

if(WIN32)
  find_library(LUA_JIT_LIBRARIES NAMES lua51 PATHS "dependencies/lua/lib")
elseif(UNIX)
  find_library(LUA_JIT_LIBRARIES NAMES luajit PATHS "dependencies/lua/lib")
endif()

include_directories(../iolite_c_api/sample_plugins/dependencies/glm/glm ../iolite_c_api dependencies dependencies/lua/include dependencies/oidn/include lua_plugin terrain_plugin)

add_library(IoliteLuaPlugin SHARED
  lua_plugin/lua_plugin.cpp
)
list(APPEND PLUGINS IoliteLuaPlugin)
add_library(IoliteTerrainPlugin SHARED
  terrain_plugin/terrain_plugin.cpp
)
list(APPEND PLUGINS IoliteTerrainPlugin)

if(WIN32)
  add_library(IoliteDenoiserOIDNPlugin SHARED
    denoiser_oidn_plugin/denoiser_oidn_plugin.cpp
  )
  list(APPEND PLUGINS IoliteDenoiserOIDNPlugin)

  find_library(_OIDN_LIB NAMES OpenImageDenoise PATHS "dependencies/oidn/lib")
  list(APPEND OIDN_LIBS ${_OIDN_LIB})
  find_library(_OIDN_LIB_TBB NAMES tbb PATHS "dependencies/oidn/lib")
  list(APPEND OIDN_LIBS ${_OIDN_LIB_TBB})
  target_link_libraries(IoliteDenoiserOIDNPlugin ${OIDN_LIBS})
endif()

target_compile_definitions(IoliteLuaPlugin PUBLIC SOL_LUAJIT=1 SOL_ALL_SAFETIES_ON=1)
target_link_libraries(IoliteLuaPlugin ${LUA_JIT_LIBRARIES})

foreach(EXECUTABLE ${PLUGINS})
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

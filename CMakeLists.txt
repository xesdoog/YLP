cmake_minimum_required(VERSION 3.15)

include(FetchContent)
include(cmake/imgui.cmake)
include(cmake/json.cmake)
include(cmake/luajit.cmake)
include(cmake/sol3.cmake)
# include(cmake/mhde.cmake)

project(YLP LANGUAGES C CXX ASM_MASM)

set(CMAKE_CXX_STANDARD 23)
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.h"
    "${SRC_DIR}/*.hpp"
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.rc"
    "${SRC_DIR}/*.asm"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

message(STATUS "Searching for required package: OpenGL...")
find_package(OpenGL REQUIRED)

message(STATUS "Setting up precompiled headers...")
target_precompile_headers(${PROJECT_NAME} PUBLIC "${SRC_DIR}/common.hpp")

message(STATUS "Setting up include directories...")
target_include_directories(${PROJECT_NAME} PRIVATE
    "${SRC_DIR}"
    "${imgui_SOURCE_DIR}"
	"${luajit_SOURCE_DIR}/src"
    "${luajit_BINARY_DIR}/src"
	# "${mhde_SOURCE_DIR}/include"
)

message(STATUS "Setting up linked libraries...")
target_link_libraries(${PROJECT_NAME} PRIVATE
	imgui
	OpenGL::GL
	nlohmann_json::nlohmann_json
	sol2::sol2
	libluajit
	# mhde_lib_x64
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

message(STATUS "Adding compile definitions...")
add_compile_definitions(
	"_CRT_SECURE_NO_WARNINGS"
	"NOMINMAX"
	"WIN32_LEAN_AND_MEAN"
)

if (WIN32)
    message(STATUS "Setting up Win32 properties...")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
  target_compile_options(${PROJECT_NAME} PRIVATE "$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else ()
  add_compile_options(-Wa,-mbig-obj)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

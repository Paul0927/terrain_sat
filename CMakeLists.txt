cmake_minimum_required(VERSION 3.15)

project(Large_Terrain)

add_compile_options(/sdl)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Large_Terrain)

set(CMAKE_BUILD_TYPE Release)
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/Large_Terrain/*.h")
file(GLOB CPPS "${PROJECT_SOURCE_DIR}/Large_Terrain/*.cpp")
list(FILTER CPPS EXCLUDE REGEX ".*old\.cpp")

add_executable(Large_Terrain ${HEADERS} ${CPPS})

file(GLOB GLSLS "${PROJECT_SOURCE_DIR}/Large_Terrain/*.glsl")
set(PS_TEXTURE "${PROJECT_SOURCE_DIR}/Large_Terrain/ps_texture_16k.png")
file(TO_NATIVE_PATH "${BUILD_DIR}/data" data_dst)
file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/Large_Terrain/data" data_src)
add_custom_command(
	TARGET Large_Terrain PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${data_dst}
	COMMAND cmd /c rmdir /s /q ${data_dst}
	COMMAND cmd /c mklink /j ${data_dst} ${data_src}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLSLS} ${BUILD_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PS_TEXTURE} ${BUILD_DIR}
)

target_include_directories(
	Large_Terrain PRIVATE
	"${PROJECT_SOURCE_DIR}/Large_Terrain"
	"${PROJECT_SOURCE_DIR}/extern/FreeImage/include"
	"${PROJECT_SOURCE_DIR}/extern/freetype2/include"
	"${PROJECT_SOURCE_DIR}/extern/glew/include"
	"${PROJECT_SOURCE_DIR}/extern/glfw/include"
	"${PROJECT_SOURCE_DIR}/extern/glm"
	"${PROJECT_SOURCE_DIR}/extern/Soil/include"
)

find_package(OpenGL REQUIRED)
target_link_libraries(Large_Terrain PRIVATE OpenGL::GL)
target_link_libraries(Large_Terrain PRIVATE OpenGL::GLU)

set(
	LIB_NAMES
	"FreeImage.lib"
	"freetype265.lib"
	"libglew32.lib"
	"glfw3.lib"
	"SOIL.lib"
)
set(
	LIB_DIRS
	"${PROJECT_SOURCE_DIR}/extern/FreeImage/lib/x64"
	"${PROJECT_SOURCE_DIR}/extern/freetype2/lib/x64"
	"${PROJECT_SOURCE_DIR}/extern/glew/lib/x64/Release"
	"${PROJECT_SOURCE_DIR}/extern/glfw/lib/x64/Release"
	"${PROJECT_SOURCE_DIR}/extern/Soil/lib/x64/Release"
)

foreach(LIB_NAME IN LISTS LIB_NAMES)
	find_library(lib-${LIB_NAME} ${LIB_NAME} PATHS ${LIB_DIRS} REQUIRED)
	target_link_libraries(Large_Terrain PRIVATE ${lib-${LIB_NAME}})
endforeach()

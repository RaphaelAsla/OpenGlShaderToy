cmake_minimum_required(VERSION 3.10)
project(Toy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(run
	COMMAND toy
    DEPENDS toy
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Used run :make from neovim
if (POLICY CMP0072)
  cmake_policy (SET CMP0072 NEW)
endif(POLICY CMP0072)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory("libs")

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE INCLUDE_FILES "includes/*.hpp")

set(MAIN_SOURCE "src/main.cpp")

add_executable(toy ${INCLUDE_FILES} ${SOURCE_FILES} ${MAIN_SOURCE})

target_link_libraries(toy
	glfw
    glad
    imgui
	OpenGL::GL
)

target_include_directories(toy SYSTEM PUBLIC "libs/stb_image")

target_compile_options(toy PRIVATE -Wall -Wextra -O3)

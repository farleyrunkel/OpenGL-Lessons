cmake_minimum_required(VERSION 3.4...3.28 FATAL_ERROR)

project(OpenGL-Lessons)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find glfw
find_package(glfw3 3.3 QUIET)
if(glfw3_FOUND)
	message(STATUS "GLFW found")
else()
    add_subdirectory(external/glfw)
    include_directories(external/glfw/include)
    include_directories(external/glfw/deps)
    link_libraries(glfw)
endif()


set(glad_DIR "${CMAKE_SOURCE_DIR}/external/glad")
# if glad dir is not found, throw an error
if(EXISTS ${glad_DIR})
    message(STATUS "GLAD found")
    include_directories(${glad_DIR}/include)
    add_library(glad ${glad_DIR}/src/glad.c)
    link_libraries(glad)
else()
    message(FATAL_ERROR "GLAD not found")
endif()

set(glad_DIR "${CMAKE_SOURCE_DIR}/external/stb")
# if glad dir is not found, throw an error
if(EXISTS ${glad_DIR})
    message(STATUS "GLAD found")
    include_directories(${glad_DIR})
else()
    message(FATAL_ERROR "GLAD not found")
endif()



add_subdirectory(lessons/01_Getting_Started)


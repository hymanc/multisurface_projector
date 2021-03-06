cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(PCL_MANIP)

find_package(PCL 1.3 REQUIRED)
find_package( OpenCV REQUIRED )

#SET(GCC_LINK_FLAGS    "-L /usr/lib -lglfw -lGLEW -lGL -lSOIL")
#SET(GCC_COM_FLAGS    "-L /usr/lib -lglfw -lGLEW -lGL -lSOIL")
#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COM_FLAGS}" )
#SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}" )

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw2)
#pkg_search_module(GLEW REQUIRED GLEW)

include_directories (${PROJECT_SOURCE_DIR}/shaderStuff)

add_subdirectory(shaderStuff)
message(${OpenCV_LIBS})

include_directories(${PCL_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
#include_directories(${GLFW_INCLUDE_DIRS})
#include_directories(${GLEW_INCLUDE_DIRS})

link_directories(${PCL_LIBRARY_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(/usr/lib/)
add_definitions(${PCL_DEFINITIONS})

add_executable(pclManipTest pclManipTest.cpp pclManipulator.cpp)
add_executable(shaderPCLTest shaderPCLTest.cpp pclManipulator.cpp)

target_link_libraries(pclManipTest ${PCL_LIBRARIES} shaderStuff ${OpenCV_LIBS}  )
target_link_libraries(shaderPCLTest ${PCL_LIBRARIES} shaderStuff ${OpenCV_LIBS} glfw GLEW)

# Main cmake file 
# COM S 557 Final Project
# James Nhan, jsnhan@iastate.edu
# Iowa State University
# ME/CS/CprE 557 Computer Graphics, Fall 2016

cmake_minimum_required(VERSION 2.6)

# Main project name is ARMaker
project(FinalProject)


set(CFILE_PATH  "${CMAKE_SOURCE_DIR}/../SDK/cmake/")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/" ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../SDK/cmake/")

message(STATUS "Source directory ${CMAKE_SOURCE_DIR}")
message(STATUS "SDK directory ${CMAKE_SOURCE_DIR}/../SDK")


# Find packages
FIND_PACKAGE(glew REQUIRED)
FIND_PACKAGE(glm REQUIRED)
FIND_PACKAGE(glfw3 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)


# Include dirs
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW3_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/../gl_common/)
include_directories(${CMAKE_SOURCE_DIR}/)

# Add all files to the configuration
file(GLOB FinalProject_SRC
	Camera.cpp
	Camera.h
	CoordinateSystem.cpp
	CoordinateSystem.h
	image_DXT.c
	image_DXT.h
	image_helper.c
	image_helper.h
	Lights.cpp
	Lights.h
	main.cpp
	Material.h
	Mesh.cpp
	Mesh.h
	Model.cpp
	Model.h
	Plane.cpp
	Plane.h
	Shader.cpp
	Shader.h
	SOIL.c
	SOIL.h
	stb_image_aug.c
	stb_image_aug.h
	stbi_DDS_aug.h
	stbi_DDS_aug_c.h
	Texture.h
	Vertex.h

)

set(HCI557_RES
	../data/shaders/texturedMultiLights.fs
    ../data/shaders/texturedMultiLights.vs
	../data/shaders/cubemap.fs
    ../data/shaders/cubemap.vs
	../data/shaders/singleTexture.fs
    ../data/shaders/singleTexture.vs
	../data/shaders/standard.fs
    ../data/shaders/standard.vs
)


#----
# SRC Groups
source_group(Resources FILES ${HCI557_RES})


# Create an executable
add_executable(FinalProject ${FinalProject_SRC})


# Add link directories
# Note required for this project
IF(WIN32)
	link_directories(${CMAKE_SOURCE_DIR}/../SDK/lib)
ELSE(WIN32)
	link_directories(${CMAKE_SOURCE_DIR}/../SDK/lib)
	link_directories(/usr/local/lib)
	link_directories(/usr/local/Cellar/glfw3/3.0.4/lib)
	link_directories(/usr/local/Cellar/glew/1.11.0/lib)
ENDIF(WIN32)


# Add libraries
target_link_libraries(FinalProject ${GLEW_LIBRARY} ${GLFW3_LIBRARY} ${OPENGL_LIBRARIES} ${OPENGL_LIBRARIES} legacy_stdio_definitions.lib libSOIL.lib assimp-vc140-mt.lib )
						
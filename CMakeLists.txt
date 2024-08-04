# cmake minimum version required
cmake_minimum_required(VERSION 3.28.3)

# ---

# compile options
add_compile_options(-ggdb3 -O0)

# ---

# set output directories for binary and libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---

# project name
project(mc_clone)

# ---

# set include directories
INCLUDE_DIRECTORIES(./include)

# ---

# add source fles
add_executable(${PROJECT_NAME} 
	./include/STB/stb_image.h # IMPORTANT HEADER ONLY LIBRARY

	# noise library
	./include/NOISE/noise1234.c
	./include/NOISE/noise1234.h

	# --- source files ---
	./src/main.c

	./src/camera.c

	./src/chunk.c
	./src/test_block.c
	./src/pointer.c
	./src/world.c
	./src/sky.c

	./src/shader.c
	./src/image.c
	./src/file.c
	# --- --- ---


	# --- headers ---
	./src/headers/camera.h

	./src/headers/test_block.h
	./src/headers/pointer.h
	./src/headers/chunk.h
	./src/headers/world.h
	./src/headers/sky.h

	./src/headers/shader.h
	./src/headers/image.h
	./src/headers/file.h
	# --- --- ---

	# --- header only ---
	./src/headers/mesh.h
	./src/headers/tex_coords.h
	# --- --- ---
)

# add cglm library
add_subdirectory(./include/deps/cglm)

# add glfw directory
add_subdirectory(./include/deps/glfw-3.4)

# add glad as a library
add_library(glad STATIC ./include/GLAD33/glad.c)

# ---

# link cglm, glfw and glad to project
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} cglm)

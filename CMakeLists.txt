# CMake Build Configuration for root of project
cmake_minimum_required(VERSION 3.18)
project(finalProject VERSION 1.0 DESCRIPTION "Final Project" LANGUAGES C)

# Compiler options (inherited by sub-folders)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -g -Werror -Wpedantic -Wextra)
add_compile_options(-fdiagnostics-color)

# Enable address sanitizer
# (Comment this out to make your code faster)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

# Add threading support (pthread)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# What folders to build
add_subdirectory(hal)  
add_subdirectory(app)

# Link with pthread library
target_link_libraries(finalProject PRIVATE Threads::Threads)


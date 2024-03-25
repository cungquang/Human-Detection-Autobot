# CMakeList.txt for HAL
#   Build a library (`hal`) which exposes the header files as "hal/*.h"
#   Use header as: #include "hal/button.h"

include_directories(hal/include)
file(GLOB MY_SOURCES "src/*.c")

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)

add_library(hal STATIC ${MY_SOURCES})

target_include_directories(hal PUBLIC include)

add_executable(camera src/camera.c)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET camera POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:camera>"
     "~/cmpt433/public/myApps/camera" 
  COMMENT "Copying ARM executable to public NFS directory")
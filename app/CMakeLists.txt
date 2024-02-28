# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(finalProject ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(finalProject LINK_PRIVATE hal)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET finalProject POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:assignment2>"
     "~/cmpt433/public/myApps/assignment2" 
  COMMENT "Copying ARM executable to public NFS directory")

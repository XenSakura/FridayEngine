cmake_minimum_required(VERSION 3.10)

project(FridayEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Add your source files here
set(SOURCES 
    main.cpp
    # Add more source files if needed
)

# Add your library files here
set(LIBRARIES
    ${PROJECT_SOURCE_DIR}/lib/tinyrací.a
    # Add more libraries if needed
)

# Create the executable
add_executable(FridayEngine ${SOURCES})

# Link the libraries to the executable
target_link_libraries(FridayEngine ${LIBRARIES})

# Install the executable
install(TARGETS FridayEngine DESTINATION bin)
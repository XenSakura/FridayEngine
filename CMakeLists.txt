cmake_minimum_required(VERSION 3.20)
project(FridayEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Vulkan
find_package(Vulkan REQUIRED)

# Add glm
add_subdirectory(Libraries/glm)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/libraries/glm
    ${Vulkan_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/engine/core
    ${CMAKE_SOURCE_DIR}/engine/graphics
    ${CMAKE_SOURCE_DIR}/engine/physics
    ${CMAKE_SOURCE_DIR}/engine/custom-data-structures
)

# List source files
file(GLOB_RECURSE ENGINE_CORE_SOURCES
    "engine/core/*.cpp"
    "engine/core/*.h"
)
file(GLOB_RECURSE ENGINE_GRAPHICS_SOURCES
    "engine/graphics/*.cpp"
    "engine/graphics/*.h"
)
file(GLOB_RECURSE ENGINE_PHYSICS_SOURCES
    "engine/physics/*.cpp"
    "engine/physics/*.h"
)
file(GLOB_RECURSE ENGINE_CUSTOM_DATA_STRUCTURES_SOURCES
    "engine/custom-data-structures/*.cpp"
    "engine/custom-data-structures/*.h"
)

# Add executable
add_executable(FridayEngine 
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_GRAPHICS_SOURCES}
    ${ENGINE_PHYSICS_SOURCES}
    ${ENGINE_CUSTOM_DATA_STRUCTURES_SOURCES}
)

# Group source files by subdirectories under "Engine"
foreach(_source IN ITEMS ${ENGINE_CORE_SOURCES} ${ENGINE_GRAPHICS_SOURCES} ${ENGINE_PHYSICS_SOURCES} ${ENGINE_CUSTOM_DATA_STRUCTURES_SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}/engine" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("engine\\${_group_path}" FILES "${_source}")
endforeach()

# Link libraries
target_link_libraries(FridayEngine glm glfw imgui ${Vulkan_LIBRARIES})


# Allow debugging
target_compile_options(${PROJECT_NAME} PUBLIC -ggdb)

# Copy assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:FridayEngine>/shaders
    COMMENT "Copy shaders to build tree"
)

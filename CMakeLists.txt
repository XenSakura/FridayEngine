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
    ${CMAKE_SOURCE_DIR}/Libraries/glm
    ${Vulkan_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/Engine/Core
    ${CMAKE_SOURCE_DIR}/Engine/Graphics
    ${CMAKE_SOURCE_DIR}/Engine/Physics
)

# List source files
file(GLOB_RECURSE ENGINE_CORE_SOURCES
    "Engine/Core/*.cpp"
    "Engine/Core/*.h"
)
file(GLOB_RECURSE ENGINE_GRAPHICS_SOURCES
    "Engine/Graphics/*.cpp"
    "Engine/Graphics/*.h"
)
file(GLOB_RECURSE ENGINE_PHYSICS_SOURCES
    "Engine/Physics/*.cpp"
    "Engine/Physics/*.h"
)

# Add executable
add_executable(FridayEngine 
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_GRAPHICS_SOURCES}
    ${ENGINE_PHYSICS_SOURCES}
)

# Group source files by subdirectories under "Engine"
foreach(_source IN ITEMS ${ENGINE_CORE_SOURCES} ${ENGINE_GRAPHICS_SOURCES} ${ENGINE_PHYSICS_SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${CMAKE_SOURCE_DIR}/Engine" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("Engine\\${_group_path}" FILES "${_source}")
endforeach()

# Link libraries
target_link_libraries(FridayEngine glm glfw imgui ${Vulkan_LIBRARIES})
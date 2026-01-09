cmake_minimum_required(VERSION 3.10)
project(VulkanExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/Model.cpp
)

set(HEADERS
    include/Model.h
    include/ModelInstance.h
    include/SceneManager.h
)

# Create executable
add_executable(vulkan_app ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(vulkan_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm-1.0.3
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb-master
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tiny_obj_loader
    ${Vulkan_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(vulkan_app PRIVATE
    ${Vulkan_LIBRARIES}
)

# Platform-specific settings
if(WIN32)
    # Add GLFW library path for Windows (MSVC)
    target_link_directories(vulkan_app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw-3.4/lib-vc2022
    )
    target_link_libraries(vulkan_app PRIVATE
        glfw3
        user32
        gdi32
        shell32
        ole32
    )
    
    # MSVC-specific settings
    if(MSVC)
        set_target_properties(vulkan_app PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        
        # Set subsystem to Windows (no console)
        target_link_options(vulkan_app PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
    endif()
elseif(UNIX)
    # Linux/macOS settings
    target_link_libraries(vulkan_app PRIVATE
        glfw
    )
endif()

# Add preprocessor definitions
target_compile_definitions(vulkan_app PRIVATE
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_ENABLE_EXPERIMENTAL
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# Optimization flags for Release build
if(MSVC)
    target_compile_options(vulkan_app PRIVATE
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Debug>:/Zi>
    )
else()
    target_compile_options(vulkan_app PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-g>
    )
endif()

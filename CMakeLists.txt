cmake_minimum_required(VERSION 3.10)
project(VulkanExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable parallel compilation for MSVC
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /FS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /FS")
endif()

# Set output directories (build in build/ subdirectory)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/Model.cpp
    src/WindowManager.cpp
    src/VulkanInitializer.cpp
    src/ExampleApplication.cpp
    src/VulkanRenderer.cpp
    src/VulkanSwapChain.cpp
    src/VulkanTexture.cpp
    src/VulkanBuffer.cpp
    src/VulkanPipeline.cpp
    src/VulkanCommandManager.cpp
    src/VulkanDescriptorManager.cpp
)

set(HEADERS
    src/headers/Camera.h
    src/headers/ExampleApplication.h
    src/headers/Material.h
    src/headers/Model.h
    src/headers/ModelInstance.h
    src/headers/SceneManager.h
    src/headers/VulkanInitializer.h
    src/headers/VulkanRenderer.h
    src/headers/VulkanSwapChain.h
    src/headers/VulkanTexture.h
    src/headers/VulkanBuffer.h
    src/headers/VulkanPipeline.h
    src/headers/VulkanCommandManager.h
    src/headers/VulkanDescriptorManager.h
    src/headers/ShaderParameter.h
)

# Create executable
add_executable(vulkan_app ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(vulkan_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
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

        # Set subsystem to Windows (no console) only for Release builds
        # Keep console for Debug builds to see validation layer output
        target_link_options(vulkan_app PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup>)

        # Fix runtime library conflicts in Debug builds
        target_link_options(vulkan_app PRIVATE $<$<CONFIG:Debug>:/NODEFAULTLIB:MSVCRT>)
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
    # Use Multi-threaded DLL runtime library
    set_property(TARGET vulkan_app PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    
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

cmake_minimum_required(VERSION 3.25)
project(VulkanTest VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only library paths
set(LOCAL_THIRD_PARTY_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb_image"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/tiny_obj_loader"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.4.bin.WIN64/include"
)

# local GLFW binary
find_library(GLFW_LIB 
    NAMES glfw3 glfw3dll
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.4.bin.WIN64/lib-vc2022" 
          "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.4.bin.WIN64/lib"
    REQUIRED
)

# Vulkan 
find_package(Vulkan REQUIRED)

find_program(GLSLC_EXECUTABLE 
    NAMES glslc 
    PATHS "$ENV{VULKAN_SDK}/Bin"
    REQUIRED
)

function(compile_shader TARGET_NAME SHADER_SOURCE OUTPUT_SPV)
    add_custom_command(
        OUTPUT "${OUTPUT_SPV}"
        COMMAND "${GLSLC_EXECUTABLE}" -O "${SHADER_SOURCE}" -o "${OUTPUT_SPV}"
        DEPENDS "${SHADER_SOURCE}"
        VERBATIM
    )
    target_sources(${TARGET_NAME} PRIVATE "${OUTPUT_SPV}")
endfunction()

# Target config

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(VulkanTest ${SOURCES} ${HEADERS})

compile_shader(VulkanTest "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert_shader.vert" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert_shader.spv")
compile_shader(VulkanTest "${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag_shader.frag" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag_shader.spv")
compile_shader(VulkanTest "${CMAKE_CURRENT_SOURCE_DIR}/instanced_shader.vert"    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/instanced_vert_shader.spv")
compile_shader(VulkanTest "${CMAKE_CURRENT_SOURCE_DIR}/instanced_shader.frag"    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/instanced_frag_shader.spv")
compile_shader(VulkanTest "${CMAKE_CURRENT_SOURCE_DIR}/skybox_shader.vert"       "${CMAKE_CURRENT_SOURCE_DIR}/shaders/skybox_vert_shader.spv")
compile_shader(VulkanTest "${CMAKE_CURRENT_SOURCE_DIR}/skybox_shader.frag"       "${CMAKE_CURRENT_SOURCE_DIR}/shaders/skybox_frag_shader.spv")

target_include_directories(VulkanTest PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LOCAL_THIRD_PARTY_INCLUDES}
)

target_link_libraries(VulkanTest PRIVATE 
    Vulkan::Vulkan
    ${GLFW_LIB}
)

# Post-build

add_custom_command(
    TARGET VulkanTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/textures" "$<TARGET_FILE_DIR:VulkanTest>/textures"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:VulkanTest>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/skybox" "$<TARGET_FILE_DIR:VulkanTest>/skybox"
)
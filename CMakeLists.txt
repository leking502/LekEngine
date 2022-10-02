cmake_minimum_required(VERSION 3.15)
message(CMAKE_VERSION: ${CMAKE_VERSION})

# To use template project:
#  Copy template project folder
#  Change VULKAN_PROJECT_NAME
set(VULKAN_PROJECT_NAME LekEngine)
project(${VULKAN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
message(CMAKE_C_COMPILER: ${CMAKE_C_COMPILER})

# Set all paths based on current operating system
if (UNIX)
    # Vulkan SDK on Linux
#    set(VULKAN_SDK_PATH /home/barry/Vulkan/1.2.131.2/x86_64)
#    include_directories(${VULKAN_SDK_PATH}/include)
#    link_directories(${VULKAN_SDK_PATH}/lib)
    link_libraries(vulkan)

    # enable pkg-config actions via CMake GLFW3_STATIC_LIBRARIES flag.
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    message(GLFW3_STATIC_LIBRARIES: ${GLFW3_STATIC_LIBRARIES})

    # pkg-config included -L/usr/local/lib, which is not in GLFW3_STATIC_LIBRARIES
    link_directories(/usr/local/lib)
    link_libraries(${GLFW3_STATIC_LIBRARIES})

else()
    # Vulkan SDK on Windows
    set(VULKAN_SDK_PATH c:/VulkanSDK/1.2.135.0)
    include_directories(${VULKAN_SDK_PATH}/include)
    link_directories(${VULKAN_SDK_PATH}/Lib)
    link_libraries(vulkan-1)

    # GLFW on Windows, without the help of pkg-config
    set(GLFW_PATH c:/GLFW_Libs)
    include_directories(${GLFW_PATH}/include)
    link_directories(${GLFW_PATH}/lib-vc2019)
    link_libraries(glfw3)
endif()


add_executable(${VULKAN_PROJECT_NAME} main.cpp lek_window.hpp lek_window.cpp lek_app.cpp lek_app.hpp lek_pipeline.cpp lek_pipeline.hpp lek_device.cpp lek_device.hpp lek_swap_chain.cpp lek_swap_chain.hpp lek_model.cpp lek_model.hpp lek_game_object.cpp lek_game_object.hpp lek_renderer.cpp lek_renderer.hpp simple_render_system.cpp simple_render_system.hpp lek_camera.cpp lek_camera.hpp keyboard_movement_controller.cpp keyboard_movement_controller.hpp maze_game.cpp maze_game.hpp ModelLoder/tiny_obj_loader.h lek_utils.hpp lek_buffer.cpp lek_buffer.hpp lek_frame_info.cpp lek_frame_info.hpp)

#file(GLOB_RECURSE GLSL_SOURCE_FILES
#        "${PROJECT_SOURCE_DIR}/shaders/*.frag"
#        "${PROJECT_SOURCE_DIR}/shaders/*.vert"
#        )
#
#foreach(GLSL ${GLSL_SOURCE_FILES})
#    get_filename_component(FILE_NAME ${GLSL} NAME)
#    set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
#    add_custom_command(
#            OUTPUT ${SPIRV}
#            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
#            DEPENDS ${GLSL})
#    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
#endforeach(GLSL)
#
#add_custom_target(
#        Shaders
#        DEPENDS ${SPIRV_BINARY_FILES}
#)
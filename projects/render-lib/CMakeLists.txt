project(render VERSION 1.0.0 DESCRIPTION "Render Library")

find_package(Vulkan REQUIRED)
file(GLOB_RECURSE RENDER_LIB_FILES "*.cpp" "*.h")

add_library(render ${RENDER_LIB_FILES})
add_library(render::render ALIAS render)

find_assign_files(${RENDER_LIB_FILES})

include_directories(../../dep/glfw/include)

target_include_directories(render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE Vulkan::Vulkan)
target_link_libraries(render PUBLIC
	asio::asio
	common::common
	glfw ${GLFW_LIBRARIES}
	Vulkan::Vulkan
)
add_dependencies(render shaders)

add_custom_command(TARGET render POST_BUILD
	COMMENT "Compiling shaders..."
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_OUTPUT}"
        "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/shaders/"
        )

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS VK_USE_PLATFORM_WIN32_KHR)

# Set VERSION & FOLDER Property
set_target_properties(render PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(render PROPERTIES FOLDER "libs")
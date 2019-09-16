project(render VERSION 1.0.0 DESCRIPTION "Render Library")

file(GLOB_RECURSE RENDER_LIB_FILES "*.cpp" "*.h")

add_library(render ${RENDER_LIB_FILES})
add_library(render::render ALIAS render)

find_assign_files(${RENDER_LIB_FILES})

target_include_directories(render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(render PUBLIC
	asio::asio
	common::common
)

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Set VERSION & FOLDER Property
set_target_properties(render PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(render PROPERTIES FOLDER "libs")
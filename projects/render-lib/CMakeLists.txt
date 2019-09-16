project(render-lib VERSION 1.0.0 DESCRIPTION "Render Library")

file(GLOB_RECURSE RENDER_LIB_FILES "*.cpp" "*.h")

add_library(render-lib ${RENDER_LIB_FILES})
add_library(render-lib::render-lib ALIAS render-lib)

find_assign_files(${RENDER_LIB_FILES})

# Set VERSION & FOLDER Property
set_target_properties(render-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(render-lib PROPERTIES FOLDER "libs")

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

target_link_libraries(render-lib PRIVATE
	asio::asio
	common::common
)

install(TARGETS render-lib DESTINATION bin)
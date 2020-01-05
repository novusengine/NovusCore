project(client-demo VERSION 1.0.0 DESCRIPTION "Wow demo client for NovusCore")

add_subdirectory(Dependencies)

file(GLOB_RECURSE CLIENT_DEMO_FILES "*.cpp" "*.h")
add_executable(client-demo ${CLIENT_DEMO_FILES})

find_assign_files(${CLIENT_DEMO_FILES})

include_directories(../../dep/glfw/include)

# Set VERSION & FOLDER Property
set_target_properties(client-demo PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(client-demo PROPERTIES FOLDER "client-demo")

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

target_link_libraries(client-demo PRIVATE
	client-demo::dependencies
	asio::asio
	common::common
	render::render
	network::network
	input::input
	glfw ${GLFW_LIBRARIES}
)

install(TARGETS client-demo DESTINATION bin)
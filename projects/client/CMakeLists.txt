project(client VERSION 1.0.0 DESCRIPTION "Wow client for NovusCore")

file(GLOB_RECURSE CLIENT_FILES "*.cpp" "*.h")

add_executable(client ${CLIENT_FILES})

find_assign_files(${CLIENT_FILES})

# Set VERSION & FOLDER Property
set_target_properties(client PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(client PROPERTIES FOLDER "client")

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

target_link_libraries(client PRIVATE
	asio::asio
	common::common
)

install(TARGETS client DESTINATION bin)
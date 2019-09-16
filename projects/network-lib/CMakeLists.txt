project(network VERSION 1.0.0 DESCRIPTION "Network Library")

file(GLOB_RECURSE NETWORK_LIB_FILES "*.cpp" "*.h")

add_library(network ${NETWORK_LIB_FILES})
add_library(network::network ALIAS network)

find_assign_files(${NETWORK_LIB_FILES})

target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(network PUBLIC
	asio::asio
	common::common
)

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Set VERSION & FOLDER Property
set_target_properties(network PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(network PROPERTIES FOLDER "libs")
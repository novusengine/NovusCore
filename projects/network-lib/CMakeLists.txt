project(network-lib VERSION 1.0.0 DESCRIPTION "Network Library")

file(GLOB_RECURSE NETWORK_LIB_FILES "*.cpp" "*.h")

add_library(network-lib ${NETWORK_LIB_FILES})
add_library(network-lib::network-lib ALIAS network-lib)

find_assign_files(${NETWORK_LIB_FILES})

# Set VERSION & FOLDER Property
set_target_properties(network-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(network-lib PROPERTIES FOLDER "network-lib")

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

target_link_libraries(network-lib PRIVATE
	asio::asio
	common::common
)

install(TARGETS network-lib DESTINATION bin)
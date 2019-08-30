project(common VERSION 1.0.0 DESCRIPTION "Common is a static library for NovusCore")

file(GLOB_RECURSE COMMON_FILES "*.cpp" "*.h")

add_subdirectory(Dependencies)

add_library(common ${COMMON_FILES})
add_library(common::common ALIAS common)

find_assign_files(${COMMON_FILES})

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common PUBLIC
    tracy::tracy
    common::dependencies
    openssl::openssl
    zlib::zlib
)

add_compile_definitions(NOMINMAX _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Set VERSION & FOLDER Property
set_target_properties(common PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(common PROPERTIES FOLDER "client")

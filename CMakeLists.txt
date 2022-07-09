if (POLICY CMP0048)
  # cmake warns if loaded from a min-3.0-required parent dir, so silence the warning:
  cmake_policy(SET CMP0048 NEW)
endif()

project(NovusCore VERSION 1.0.0)

# CMake >= 3.12
cmake_minimum_required(VERSION 3.12)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE STRING "" FORCE)
set(NOVUSCORE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(NOVUSCORE_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

configure_file(
        ${NOVUSCORE_SOURCE_DIR}/CookData.bat.in
        ${NOVUSCORE_BUILD_DIR}/CookData.bat

        @ONLY
    )

add_subdirectory(Submodules)
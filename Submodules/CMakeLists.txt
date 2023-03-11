# Include Project Directories
if (${NOVUSCORE_BUILD_ENGINE})
add_subdirectory(Engine)
endif()

if (${NOVUSCORE_BUILD_SERVER})
    add_subdirectory(Server)
endif()

if (${NOVUSCORE_BUILD_GAME})
add_subdirectory(Game)
endif()
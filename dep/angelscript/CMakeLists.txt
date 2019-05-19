cmake_minimum_required(VERSION 3.5)

project(angelscript)

option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(AS_NO_EXCEPTIONS "Disable exception handling in script context" OFF)

if(APPLE)
    option(BUILD_FRAMEWORK "Build Framework bundle for OSX" OFF)
endif()

file(READ include/angelscript.h ANGELSCRIPT_H)
string(REGEX MATCH "#define ANGELSCRIPT_VERSION_STRING \"([0-9]*).([0-9]*).([0-9]*)" ANGELSCRIPT_VERSION_REGEX ${ANGELSCRIPT_H})
set(ANGELSCRIPT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(ANGELSCRIPT_VERSION_MINOR ${CMAKE_MATCH_2})
set(ANGELSCRIPT_VERSION_PATCH ${CMAKE_MATCH_3})
set(PROJECT_VERSION ${ANGELSCRIPT_VERSION_MAJOR}.${ANGELSCRIPT_VERSION_MINOR}.${ANGELSCRIPT_VERSION_PATCH})

message(STATUS "Configuring angelscript ${PROJECT_VERSION}")

find_package(Threads)

set(ANGELSCRIPT_HEADERS
    include/angelscript.h
    angelscript/as_array.h
    angelscript/as_builder.h
    angelscript/as_bytecode.h
    angelscript/as_callfunc.h
    angelscript/as_compiler.h
    angelscript/as_config.h
    angelscript/as_configgroup.h
    angelscript/as_context.h
    angelscript/as_criticalsection.h
    angelscript/as_datatype.h
    angelscript/as_debug.h
    angelscript/as_generic.h
    angelscript/as_map.h
    angelscript/as_memory.h
    angelscript/as_module.h
    angelscript/as_objecttype.h
    angelscript/as_outputbuffer.h
    angelscript/as_parser.h
    angelscript/as_property.h
    angelscript/as_restore.h
    angelscript/as_scriptcode.h
    angelscript/as_scriptengine.h
    angelscript/as_scriptfunction.h
    angelscript/as_scriptnode.h
    angelscript/as_scriptobject.h
    angelscript/as_string.h
    angelscript/as_string_util.h
    angelscript/as_texts.h
    angelscript/as_thread.h
    angelscript/as_tokendef.h
    angelscript/as_tokenizer.h
    angelscript/as_typeinfo.h
    angelscript/as_variablescope.h
)

set(ANGELSCRIPT_SOURCE
    angelscript/as_atomic.cpp
    angelscript/as_builder.cpp
    angelscript/as_bytecode.cpp
    angelscript/as_callfunc.cpp
    angelscript/as_callfunc_mips.cpp
    angelscript/as_callfunc_x86.cpp
    angelscript/as_callfunc_x64_gcc.cpp
    angelscript/as_callfunc_x64_msvc.cpp
    angelscript/as_callfunc_x64_mingw.cpp
    angelscript/as_compiler.cpp
    angelscript/as_configgroup.cpp
    angelscript/as_context.cpp
    angelscript/as_datatype.cpp
    angelscript/as_gc.cpp
    angelscript/as_generic.cpp
    angelscript/as_globalproperty.cpp
    angelscript/as_memory.cpp
    angelscript/as_module.cpp
    angelscript/as_objecttype.cpp
    angelscript/as_outputbuffer.cpp
    angelscript/as_parser.cpp
    angelscript/as_restore.cpp
    angelscript/as_scriptcode.cpp
    angelscript/as_scriptengine.cpp
    angelscript/as_scriptfunction.cpp
    angelscript/as_scriptnode.cpp
    angelscript/as_scriptobject.cpp
    angelscript/as_string.cpp
    angelscript/as_string_util.cpp
    angelscript/as_thread.cpp
    angelscript/as_tokenizer.cpp
    angelscript/as_typeinfo.cpp
    angelscript/as_variablescope.cpp
)

if(MSVC AND CMAKE_CL_64)
	if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		enable_language(ASM_MASM)
		if(CMAKE_ASM_MASM_COMPILER_WORKS)
			set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/as_callfunc_x64_msvc_asm.asm)
		else()
			message(FATAL ERROR "MSVC x86_64 target requires a working assembler")
		endif()
	endif()
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
    enable_language(ASM)
    if(CMAKE_ASM_COMPILER_WORKS)
        set(ANGELSCRIPT_SOURCE ${ANGELSCRIPT_SOURCE} angelscript/as_callfunc_arm.cpp angelscript/as_callfunc_arm_gcc.S)
        set_property(SOURCE angelscript/as_callfunc_arm_gcc.S APPEND PROPERTY COMPILE_FLAGS " -Wa,-mimplicit-it=always")
    else()
        message(FATAL ERROR "ARM target requires a working assembler")
    endif()
endif()

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")  
endif()

if(NOT BUILD_FRAMEWORK)
    set(ANGELSCRIPT_LIBRARY_NAME angelscript)
else()
    set(ANGELSCRIPT_LIBRARY_NAME Angelscript) # OS X frameworks should have capitalized name
    set(BUILD_SHARED_LIBS TRUE)
endif()
set(ANGELSCRIPT_LIBRARY_NAME ${ANGELSCRIPT_LIBRARY_NAME} CACHE STRING "" FORCE)

add_library(${ANGELSCRIPT_LIBRARY_NAME} ${ANGELSCRIPT_SOURCE} ${ANGELSCRIPT_HEADERS})

#target_include_directories(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

if(MSVC)
    target_compile_definitions(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)    
endif()

target_compile_definitions(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE -DANGELSCRIPT_EXPORT -D_LIB)

if(AS_NO_EXCEPTIONS)
	target_compile_definitions(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE AS_NO_EXCEPTIONS)
endif()

# Fix x64 issues on Linux
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" AND UNIX AND NOT APPLE)
    target_compile_options(${ANGELSCRIPT_LIBRARY_NAME} PRIVATE -fPIC)
endif()

# Don't override the default library output path to avoid conflicts when building for multiple target platforms
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../lib)
target_link_libraries(${ANGELSCRIPT_LIBRARY_NAME} Threads::Threads)

set_target_properties(${ANGELSCRIPT_LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

if(BUILD_FRAMEWORK)
    set_target_properties(${ANGELSCRIPT_LIBRARY_NAME} PROPERTIES
        FRAMEWORK TRUE
        FRAMEWORK_VERSION ${PROJECT_VERSION}
        MACOSX_FRAMEWORK_IDENTIFIER com.angelcode.Angelscript
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION}
        XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
        PUBLIC_HEADER include/angelscript.h
    )
endif()

#if(MSVC)
#    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#        set_target_properties(${ANGELSCRIPT_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS "/MP")
#    endif()
#endif()

if (MSVC)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") #MSVC
        remove_definitions("/MP")
    endif()
endif()

# Don't override the default runtime output path to avoid conflicts when building for multiple target platforms
#set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)

#See https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#creating-packages for a detailed explanation about this part
install(TARGETS ${ANGELSCRIPT_LIBRARY_NAME} EXPORT AngelscriptTargets
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)

install(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/include/angelscript.h
	DESTINATION include
	COMPONENT Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/Angelscript/AngelscriptConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

export(EXPORT AngelscriptTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/Angelscript/AngelscriptTargets.cmake"
	NAMESPACE Angelscript::
)
configure_file(cmake/AngelscriptConfig.cmake
	"${CMAKE_CURRENT_BINARY_DIR}/Angelscript/AngelscriptConfig.cmake"
	COPYONLY
)

set(ConfigPackageLocation lib/cmake/Angelscript)
install(EXPORT AngelscriptTargets
	FILE AngelscriptTargets.cmake
	NAMESPACE Angelscript::
	DESTINATION ${ConfigPackageLocation}
)
install(
	FILES
		cmake/AngelscriptConfig.cmake
		"${CMAKE_CURRENT_BINARY_DIR}/Angelscript/AngelscriptConfigVersion.cmake"
	DESTINATION ${ConfigPackageLocation}
	COMPONENT Devel
)

set_target_properties(angelscript PROPERTIES FOLDER "dep")
set_target_properties(angelscript PROPERTIES LINK_FLAGS "/ignore:4221")
set_property(TARGET angelscript PROPERTY CXX_STANDARD 17)
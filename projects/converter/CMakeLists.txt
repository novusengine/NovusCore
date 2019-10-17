project(converter VERSION 1.0.0 DESCRIPTION "Converter for internal filetype" LANGUAGES CSharp)

# Because CMake has really bad CSharp support, we're going to generate our own .csproj file from our .csproj template
# We then include this .csproj file with include_external_msproject

#First make CSHARP_SOURCE_FILES_LIST include all .cs files
set(CSHARP_SOURCE_FILES_LIST "")
file(GLOB_RECURSE CSHARP_SOURCE_FILES "*.cs")
foreach(FILE_PATH ${CSHARP_SOURCE_FILES})
	# Get the link path, we do this by finding the first instance of "${PROJECT_NAME}/" and substringing everything after it
	string(FIND ${FILE_PATH} ${PROJECT_NAME} LINK_START_POS)
	math(EXPR LINK_START_POS "${LINK_START_POS}+10")
	string(SUBSTRING ${FILE_PATH} ${LINK_START_POS} -1 FILE_LINK)

	# We replace all frontslashes with backslashes because appearantly that's what msbuild wants
	string(REGEX REPLACE "/" "\\\\" FILE_LINK ${FILE_LINK}) 
	string(REGEX REPLACE "/" "\\\\" FILE_PATH ${FILE_PATH}) 

	# We append <Compile Include="${FILE_PATH}" Link="${FILE_LINK} /> \n" to CSHARP_SOURCE_FILES_LIST
	# But first we append a newline if it's not the first appended Compile
	if (NOT ${CSHARP_SOURCE_FILES_LIST} STREQUAL "")
		set(CSHARP_SOURCE_FILES_LIST "${CSHARP_SOURCE_FILES_LIST} \n\t\t")
	endif()

	set(CSHARP_SOURCE_FILES_LIST "${CSHARP_SOURCE_FILES_LIST} <Compile Include=\"${FILE_PATH}\" Link=\"${FILE_LINK}\" />")
endforeach(FILE_PATH)

# Create a variable to hold the path to our packages.config
set(CSHARP_PACKAGES_CONFIG "")

# Then make CSHARP_APP_CONFIG_FILES_LIST include all .config files
set(CSHARP_APP_CONFIG_FILES_LIST "")
file(GLOB_RECURSE CSHARP_APP_CONFIG_FILES "*.config")
foreach(FILE_PATH ${CSHARP_APP_CONFIG_FILES})
	# Get the link path, we do this by finding the first instance of ${PROJECT_NAME} and substringing everything after it
	string(FIND ${FILE_PATH} ${PROJECT_NAME} LINK_START_POS)
	math(EXPR LINK_START_POS "${LINK_START_POS}+10")
	string(SUBSTRING ${FILE_PATH} ${LINK_START_POS} -1 FILE_LINK)

	# We replace all frontslashes with backslashes because appearantly that's what msbuild wants
	string(REGEX REPLACE "/" "\\\\" FILE_LINK ${FILE_LINK}) 
	string(REGEX REPLACE "/" "\\\\" FILE_PATH ${FILE_PATH}) 

	# We append <None Include="${FILE_PATH}" Link="${FILE_LINK} /> \n" to CSHARP_APP_CONFIG_FILES_LIST
	# But first we append a newline if it's not the first appended Compile
	if (NOT ${CSHARP_APP_CONFIG_FILES_LIST} STREQUAL "")
		set(CSHARP_APP_CONFIG_FILES_LIST "${CSHARP_APP_CONFIG_FILES_LIST} \n\t\t")
	endif()

	set(CSHARP_APP_CONFIG_FILES_LIST "${CSHARP_APP_CONFIG_FILES_LIST} <None Include=\"${FILE_PATH}\" Link=\"${FILE_LINK}\" />")

	# Lastly check if this was packages.config, if it was we set CSHARP_PACKAGES_CONFIG
	string(FIND ${FILE_PATH} "packages.config" PACKAGES_CONFIG_POS)
	if (NOT ${PACKAGES_CONFIG_POS} STREQUAL "-1")
		set(CSHARP_PACKAGES_CONFIG ${FILE_PATH})
	endif()

endforeach(FILE_PATH)

# Make an alternative CMAKE_INSTALL_PREFIX which uses backslashes
string(REGEX REPLACE "/" "\\\\" CMAKE_INSTALL_PREFIX_BACKSLASHES ${CMAKE_INSTALL_PREFIX}) 
#set(CMAKE_INSTALL_PREFIX_BACKSLASHES "..")

# Then we configure our template file, which will copy it to our install prefix and replace all matched ${} tags
configure_file(../../cmake/templates/CSharp.csproj.template "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.csproj")
# If we found packages.config we copy it from sources to be next to the .csproj in the install prefix
if (NOT ${CSHARP_PACKAGES_CONFIG} STREQUAL "")
	file(COPY ${CSHARP_PACKAGES_CONFIG}
	DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

# Then we include the .csproj file into this project
include_external_msproject(
	${PROJECT_NAME} ${PROJECT_NAME}.csproj
	TYPE FAE04EC0-301F-11D3-BF4B-00C04F79EFBC)
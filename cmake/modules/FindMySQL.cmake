#[==[
Provides the following variables:

  * `MySQL_INCLUDE_DIRS`: Include directories necessary to use MySQL.
  * `MySQL_LIBRARIES`: Libraries necessary to use MySQL.
  * A `MySQL::MySQL` imported target.
#]==]

# No .pc files are shipped with MySQL on Windows.
include(FindPackageHandleStandardArgs)

set(_MySQL_use_pkgconfig 0)
if (NOT WIN32)
  find_package(PkgConfig)
  if (PkgConfig_FOUND)
    set(_MySQL_use_pkgconfig 1)
  endif ()
endif ()

set(MySQL_FOUND 0)

# Try finding package via pkgconfig
if (_MySQL_use_pkgconfig)
  pkg_check_modules(_mariadb "mariadb" QUIET IMPORTED_TARGET)
  unset(_mysql_target)
  if (NOT _mariadb_FOUND)
    pkg_check_modules(_mysql "mysql" QUIET IMPORTED_TARGET)
    if (_mysql_FOUND)
      set(_mysql_target "_mysql")
    endif ()
  else ()
    set(_mysql_target "_mariadb")
    if (_mariadb_VERSION VERSION_LESS 10.4)
      get_property(_include_dirs
        TARGET    "PkgConfig::_mariadb"
        PROPERTY  "INTERFACE_INCLUDE_DIRECTORIES")
      # Remove "${prefix}/mariadb/.." from the interface since it breaks other
      # projects.
      list(FILTER _include_dirs EXCLUDE REGEX "\\.\\.")
      set_property(TARGET "PkgConfig::_mariadb"
        PROPERTY
          "INTERFACE_INCLUDE_DIRECTORIES" "${_include_dirs}")
      unset(_include_dirs)
    endif ()
  endif ()

  if (_mysql_target)
    add_library(MySQL::MySQL INTERFACE IMPORTED)
    target_link_libraries(MySQL::MySQL
      INTERFACE "PkgConfig::${_mysql_target}")
    set(MySQL_INCLUDE_DIRS ${${_mysql_target}_INCLUDE_DIRS})
    set(MySQL_LIBRARIES ${${_mysql_target}_LINK_LIBRARIES})

    find_package_handle_standard_args(MySQL
      REQUIRED_VARS MySQL_INCLUDE_DIRS MySQL_LIBRARIES)
  endif ()
  unset(_mysql_target)
endif()

if (NOT MySQL_FOUND)
  set(_MySQL_mariadb_versions 10.2 10.3)
  set(_MySQL_versions 5.0)
  set(_MySQL_paths)
  foreach (_MySQL_version IN LISTS _MySQL_mariadb_versions)
    list(APPEND _MySQL_paths
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MariaDB ${_MySQL_version};INSTALLDIR]"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MariaDB ${_MySQL_version} (x64);INSTALLDIR]")
  endforeach ()
  foreach (_MySQL_version IN LISTS _MySQL_versions)
    list(APPEND _MySQL_paths
      "C:/Program Files/MySQL/MySQL Server ${_MySQL_version}/lib/opt"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MySQL AB\\MySQL Server ${_MySQL_version};Location]"
      "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\MySQL AB\\MySQL Server ${_MySQL_version};Location]")
  endforeach ()
  unset(_MySQL_version)
  unset(_MySQL_versions)
  unset(_MySQL_mariadb_versions)

  IF (WIN32)
    IF (CMAKE_BUILD_TYPE STREQUAL Debug)
      SET(libsuffixDist debug)
      SET(libsuffixBuild Debug)
    ELSE (CMAKE_BUILD_TYPE STREQUAL Debug)
      SET(libsuffixDist opt)
      SET(libsuffixBuild Release)
      ADD_DEFINITIONS(-DDBUG_OFF)
    ENDIF (CMAKE_BUILD_TYPE STREQUAL Debug)
  ENDIF (WIN32)

  find_path(MySQL_INCLUDE_DIR
    NAMES mysql.h
    PATHS
      "/usr/include/mysql"
      "/usr/local/include/mysql"
      "/opt/mysql/mysql/include"
      "/opt/mysql/mysql/include/mysql"
      "/opt/mysql/include"
      "/opt/local/include/mysql5"
      "/usr/local/mysql/include"
      "/usr/local/mysql/include/mysql"
      "$ENV{ProgramFiles}/MySQL/*/include"
      "$ENV{SystemDrive}/MySQL/*/include"
      ${_MySQL_paths}
    PATH_SUFFIXES include include/mysql
    DOC "Location of mysql.h")
  mark_as_advanced(MySQL_INCLUDE_DIR)
  find_library(MySQL_LIBRARY
    NAMES libmariadb mysql libmysql mysqlclient
    PATHS
      "$ENV{MYSQL_DIR}/lib/${libsuffixDist}"
      "$ENV{MYSQL_DIR}/libmysql"
      "$ENV{MYSQL_DIR}/libmysql/${libsuffixBuild}"
      "$ENV{MYSQL_DIR}/client/${libsuffixBuild}"
      "$ENV{MYSQL_DIR}/libmysql/${libsuffixBuild}"
      "$ENV{ProgramFiles}/MySQL/*/lib/${libsuffixDist}"
      "$ENV{SystemDrive}/MySQL/*/lib/${libsuffixDist}"
      "/usr/lib/mysql"
      "/usr/local/lib/mysql"
      "/usr/local/mysql/lib"
      "/usr/local/mysql/lib/mysql"
      "/opt/local/mysql5/lib"
      "/opt/local/lib/mysql5/mysql"
      "/opt/mysql/mysql/lib/mysql"
      "/opt/mysql/lib/mysql"
      ${_MySQL_paths}
    PATH_SUFFIXES lib lib/opt
    DOC "Location of the mysql library")
  mark_as_advanced(MySQL_LIBRARY)

  find_package_handle_standard_args(MySQL
    REQUIRED_VARS MySQL_INCLUDE_DIR MySQL_LIBRARY)

  if (MySQL_FOUND)
    add_library(MySQL::MySQL UNKNOWN IMPORTED)
    set_target_properties(MySQL::MySQL PROPERTIES
      IMPORTED_LOCATION "${MySQL_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${MySQL_INCLUDE_DIR}")
    set(MySQL_INCLUDE_DIRS "${MySQL_INCLUDE_DIR}")
    set(MySQL_LIBRARIES "${MySQL_LIBRARY}")
  endif ()
endif ()
unset(_MySQL_use_pkgconfig)

#[=======================================================================[.rst:
UPCXXConfigVersion
-------------------

Version helper for for UPC++ CMake package.

This module tries to find the upcxx-meta utility in the following locations:
 * $ENV{UPCXX_INSTALL}/bin, if set
 * $ENV{UPCXX_DIR}/bin, if set
 * The bin directory for the prefix where this CMake package file is located
 * Anywhere in $ENV{PATH}

#]=======================================================================]


cmake_minimum_required( VERSION 3.6 )

option(UPCXX_VERBOSE "Verbose UPC++ detection" OFF)
function(UPCXX_VERB MESSAGE)
  if (UPCXX_VERBOSE OR DEFINED ENV{UPCXX_VERBOSE} )
    message(STATUS "${MESSAGE}")
  endif()
endfunction()

# Find upcxx-meta, taking hints into account
if(DEFINED ENV{UPCXX_INSTALL} ) # recommended best-practice for UPC++ installs
  find_program( UPCXX_META_EXECUTABLE upcxx-meta HINTS "$ENV{UPCXX_INSTALL}/bin" NO_DEFAULT_PATH )
elseif(DEFINED ENV{UPCXX_DIR} ) # CMake's find_package auto variable
  find_program( UPCXX_META_EXECUTABLE upcxx-meta HINTS "$ENV{UPCXX_DIR}/bin" NO_DEFAULT_PATH )
elseif(DEFINED CMAKE_CURRENT_LIST_DIR AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../bin/upcxx-meta" )
  find_program( UPCXX_META_EXECUTABLE upcxx-meta HINTS "${CMAKE_CURRENT_LIST_DIR}/../../../bin" NO_DEFAULT_PATH )
else() # Use PATH
  find_program( UPCXX_META_EXECUTABLE upcxx-meta )
endif()
if (NOT EXISTS "${UPCXX_META_EXECUTABLE}")
  message(WARNING "Failed to find UPC++ command interface 'upcxx-meta'. Please set UPCXX_INSTALL=/path/to/upcxx or add /path/to/upcxx/bin to $PATH")
endif()


if( UPCXX_META_EXECUTABLE )
  message(STATUS "Found UPCXX: ${UPCXX_META_EXECUTABLE}")

  if ((NOT DEFINED ENV{UPCXX_CODEMODE}) OR ("$ENV{UPCXX_CODEMODE}" STREQUAL "unset"))
    string(TOUPPER "${CMAKE_BUILD_TYPE}" uc_CMAKE_BUILD_TYPE)
    if (uc_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
      set(ENV{UPCXX_CODEMODE} "debug" )
    else()
      set(ENV{UPCXX_CODEMODE} "O3" )
    endif()
    unset(uc_CMAKE_BUILD_TYPE)
  endif()
  if (NOT DEFINED ENV{UPCXX_THREADMODE} )
    set(ENV{UPCXX_THREADMODE} "seq" )
  endif()

  execute_process( COMMAND ${UPCXX_META_EXECUTABLE} CPPFLAGS OUTPUT_VARIABLE UPCXX_CPPFLAGS
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(STRIP ${UPCXX_CPPFLAGS} UPCXX_CPPFLAGS)

  #now separate include dirs from flags
  if(UPCXX_CPPFLAGS)
    string(REGEX REPLACE "[ ]+" ";" UPCXX_CPPFLAGS ${UPCXX_CPPFLAGS})
    foreach( option ${UPCXX_CPPFLAGS} )
      string(STRIP ${option} option)
      string(REGEX MATCH "^-I" UPCXX_INCLUDE ${option})
      if( UPCXX_INCLUDE )
        string( REGEX REPLACE "^-I" "" option ${option} )
        list( APPEND UPCXX_INCLUDE_DIRS ${option})
      endif()
    endforeach()
  endif()
endif()

foreach( dir ${UPCXX_INCLUDE_DIRS} )
  if( EXISTS ${dir}/upcxx/upcxx.hpp )
    set( version_pattern 
      "#[\t ]*define[\t ]+UPCXX_VERSION[\t ]+([0-9]+)"
      )
    #UPCXX_VERB("checking ${dir}/upcxx/upcxx.hpp for ${version_pattern}" )
    file( STRINGS ${dir}/upcxx/upcxx.hpp upcxx_version
      REGEX ${version_pattern} )
    #UPCXX_VERB("upcxx_version ${upcxx_version}" )

    if( ${upcxx_version} MATCHES ${version_pattern} )
      set(UPCXX_VERSION_STRING ${CMAKE_MATCH_1})
    endif()

  endif()
endforeach()

if(UPCXX_VERSION_STRING)
  UPCXX_VERB("UPCXX_VERSION_STRING: ${UPCXX_VERSION_STRING}")
  if( ${UPCXX_VERSION_STRING} MATCHES "([0-9][0-9][0-9][0-9])([0-9][0-9])([0-9][0-9])" )
      set(UPCXX_VERSION_MAJOR ${CMAKE_MATCH_1})
      set(UPCXX_VERSION_MINOR ${CMAKE_MATCH_2})
      set(UPCXX_VERSION_PATCH ${CMAKE_MATCH_3})
      string(REGEX REPLACE "^0([0-9])" "\\1" UPCXX_VERSION_MINOR "${UPCXX_VERSION_MINOR}")
      string(REGEX REPLACE "^0([0-9])" "\\1" UPCXX_VERSION_PATCH "${UPCXX_VERSION_PATCH}")
      set(PACKAGE_VERSION "${UPCXX_VERSION_MAJOR}.${UPCXX_VERSION_MINOR}.${UPCXX_VERSION_PATCH}")
      UPCXX_VERB("UPCXX_VERSION: ${PACKAGE_VERSION}")

      if (PACKAGE_VERSION VERSION_EQUAL PACKAGE_FIND_VERSION)
        set(PACKAGE_VERSION_EXACT true)
        set(PACKAGE_VERSION_COMPATIBLE true)
      elseif (PACKAGE_VERSION VERSION_GREATER PACKAGE_FIND_VERSION)
        set(PACKAGE_VERSION_COMPATIBLE true)
      endif()
  endif()
endif()
if (NOT DEFINED PACKAGE_VERSION_COMPATIBLE)
  set(PACKAGE_VERSION_UNSUITABLE true)
endif()


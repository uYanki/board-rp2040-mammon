# Build number increment
# Github: https://github.com/nandee95/Auto_Build_Number_Increment

# Check language
get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)

# Check input
set(AVAILABLE_LANGUAGES CXX;C)

IF(NOT LANGUAGE)
	message("Error: The target language not defined!")
	return(1)
ENDIF()

# This is required for "IN_LIST"
cmake_policy(SET CMP0057 NEW)
IF(NOT LANGUAGE IN_LIST AVAILABLE_LANGUAGES)
	message("version.cmake : The target language not supported!")
	return(1)
ENDIF()

IF(NOT HEADER_DIR)
	message("version.cmake : Header directory is not set!")
	return(1)
ENDIF()

IF(NOT EXISTS ${HEADER_DIR})
	message("version.cmake : Header directory not found!")
	return(1)
ENDIF()

IF(NOT CACHE_DIR)
	message("version.cmake : Cache directory is not set!")
	return(1)
ENDIF()

IF(NOT EXISTS ${CACHE_DIR})
	message("version.cmake : Cache directory not found!")
	return(1)
ENDIF()

IF(NOT HEADER_FILENAME)
	set(HEADER_FILENAME "version")
ENDIF()

IF(NOT HEADER_MACRO)
	set(HEADER_MACRO "VERSION_NUMBER")
ENDIF()

IF(NOT HEADER_GUARD)
	set(HEADER_GUARD "CMAKE_VERSION_NUMBER_HEADER")
endif()

IF(NOT CACHE_FILENAME)
	set(CACHE_FILENAME "CMakeBuildNumberCache.txt")
ENDIF()

# Basic definitions
if(${LANGUAGE} STREQUAL "CXX")
	set(HEADER_EXTENSION "hpp")
ELSEIF(${LANGUAGE} STREQUAL "C")
	set(HEADER_EXTENSION "h")
ELSE()
	message("version.cmake : Internal error")
	return(1)
ENDIF()

set(HEADER_FILE "${HEADER_DIR}/${HEADER_FILENAME}.${HEADER_EXTENSION}")
set(CACHE_FILE "${CACHE_DIR}/${CACHE_FILENAME}")

# Reading data from file + incrementation
IF(EXISTS ${CACHE_FILE})
	file(READ ${CACHE_FILE} VERSION_NUMBER)
	math(EXPR VERSION_NUMBER "${VERSION_NUMBER}+1")
ELSE()
	set(VERSION_NUMBER "1")
ENDIF()

# Update the cache
file(WRITE ${CACHE_FILE} "${VERSION_NUMBER}")

# Create the header
file(WRITE ${HEADER_FILE} "// This file is automatically generated by version.cmake\n\n#ifndef ${HEADER_GUARD}\n#define ${HEADER_GUARD}\n\n#define ${HEADER_MACRO} ${VERSION_NUMBER}\n\n#endif")

# Feedback
message("Version number: ${VERSION_NUMBER}")
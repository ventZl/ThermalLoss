CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ThermalLoss)

add_definitions(-DSOURCE_DIR=${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src /usr/X11/include)
link_directories(/usr/X11/lib)

set(IMAGE_LIBS image_common)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -arch ppc7400 -ggdb3")

message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
add_definitions(-DWITH_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR})

unset(USE_ALTIVEC)
unset(USE_SSE)
unset(USE_SCALAR)

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "powerpc")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	if(USE_VECTOR EQUAL 1)
		set(IMAGE_LIBS ${IMAGE_LIBS} image_vector)
		set(USE_ALTIVEC 1)
		add_definitions(-DWITH_ALTIVEC)
	else(USE_VECTOR EQUAL 1)
		set(USE_SCALAR 1)
		set(IMAGE_LIBS ${IMAGE_LIBS} image_scalar)
	endif()
endif()
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
	if (USE_VECTOR EQUAL 1)
		set(IMAGE_LIBS ${IMAGE_LIBS} image_vector)
		message(STATUS "Using Intel SSE3 acceleration") 
		set(USE_SSE 1)
		add_definitions(-DWITH_SSE)
	else(USE_VECTOR EQUAL 1)
		set(USE_SCALAR 1)
		set(IMAGE_LIBS ${IMAGE_LIBS} image_scalar)
	endif()
endif()

# vsade okrem windozov. TODO
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#	set(wxWidgets_CONFIG_OPTIONS --debug=yes)
#else()
#	set(wxWidgets_CONFIG_OPTIONS --debug=no)
#endif()

find_package(wxWidgets COMPONENTS core base REQUIRED)

include( "${wxWidgets_USE_FILE}" )

include(cmake/XBundleHelper.cmake)

SET(plugin_dest_dir bin)
SET(qtconf_dest_dir bin)
SET(DIRS ${wxWidgets_LIBRARY_DIRS})

#include_directories( ${CMAKE_SOURCE_DIR}/contrib/pugixml )

add_subdirectory(src)

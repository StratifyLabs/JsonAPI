cmake_minimum_required (VERSION 3.12)

if(NOT SOS_SDK_PATH)
	set(SOS_SDK_PATH $ENV{SOS_SDK_PATH})
endif()

set(CMAKE_MODULE_PATH ${SOS_SDK_PATH}/Tools/gcc/cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/sos-toolchain.cmake)
include(sos-sdk)

set(LOCAL_NAME JsonAPI)
project(${LOCAL_NAME} CXX)

sos_sdk_add_subdirectory(PRIVATE_SOURCES ${CMAKE_SOURCE_DIR}/src)
sos_sdk_add_subdirectory(INTERFACE_SOURCES ${CMAKE_SOURCE_DIR}/include)

include(targets/API_release_v7m)
include(targets/VarAPI_release_v7m)
include(targets/PrinterAPI_release_v7m)
include(targets/ChronoAPI_release_v7m)
include(targets/FsAPI_release_v7m)

sos_sdk_library_target(RELEASE ${LOCAL_NAME} "" release ${SOS_ARCH})

add_library(${RELEASE_TARGET} STATIC)

set_property(TARGET ${RELEASE_TARGET} PROPERTY CXX_STANDARD 17)

target_sources(${RELEASE_TARGET}
	PUBLIC
	PRIVATE
	${INTERFACE_SOURCES}
	${PRIVATE_SOURCES}
	)

target_compile_definitions(${RELEASE_TARGET}
	PUBLIC
	PRIVATE
	)

target_compile_options(${RELEASE_TARGET}
	PRIVATE
	-Os
	)

target_include_directories(${RELEASE_TARGET}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/ChartAPI>
	PRIVATE
	)

target_link_libraries(${RELEASE_TARGET}
	PUBLIC
	ChartAPI_release_v7m
	FsAPI_release_v7m
	ChronoAPI_release_v7m
	VarAPI_release_v7m
	)



option(BUILD_DEBUG "Build debug version of library" OFF)
if(BUILD_DEBUG)
sos_sdk_library_target(DEBUG ${LOCAL_NAME} "${LIB_OPTION}" debug ${SOS_ARCH})
add_library(${DEBUG_TARGET} STATIC)
sos_sdk_copy_target(${RELEASE_TARGET} ${DEBUG_TARGET})
sos_sdk_library_add_arch_targets("${DEBUG_OPTIONS}" ${SOS_ARCH})
endif()

sos_sdk_library("${RELEASE_OPTIONS}")
#sos_sdk_library_add_arch_targets("${RELEASE_OPTIONS}" ${SOS_ARCH} VarAPI_release)

install(DIRECTORY include/ DESTINATION include/${LOCAL_NAME} PATTERN CMakelists.txt EXCLUDE)


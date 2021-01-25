cmake_minimum_required (VERSION 3.12)

if(NOT SOS_SDK_PATH)
	set(SOS_SDK_PATH $ENV{SOS_SDK_PATH})
endif()

set(CMAKE_MODULE_PATH ${SOS_SDK_PATH}/cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/sos-toolchain.cmake)
include(sos-sdk)

set(GIT_REPOSITORY "https://github.com/akheron/jansson.git")
set(GIT_BRANCH "2.12")
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/jansson)
	message(STATUS "Need to clone for the first call to cmake")
	execute_process(
		COMMAND git clone --branch ${GIT_BRANCH} ${GIT_REPOSITORY}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
endif()

project(jansson C)

set(SOURCES_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/jansson/src)


set(PRIVATE_SOURCELIST
	${SOURCES_PREFIX}/dump.c
	${SOURCES_PREFIX}/error.c
	${SOURCES_PREFIX}/hashtable.c
	${SOURCES_PREFIX}/hashtable_seed.c
	${SOURCES_PREFIX}/load.c
	${SOURCES_PREFIX}/memory.c
	${SOURCES_PREFIX}/pack_unpack.c
	${SOURCES_PREFIX}/strbuffer.c
	${SOURCES_PREFIX}/strconv.c
	${SOURCES_PREFIX}/utf.c
	${SOURCES_PREFIX}/value.c
	${SOURCES_PREFIX}/hashtable.h
	${SOURCES_PREFIX}/jansson_private.h
	${SOURCES_PREFIX}/lookup3.h
	${SOURCES_PREFIX}/strbuffer.h
	${SOURCES_PREFIX}/utf.h
	jansson_private_config.h
	jansson_api.h
	jansson_api.c
)

set(PUBLIC_SOURCELIST
	${SOURCES_PREFIX}/jansson.h
	jansson_api.h
	jansson_config.h
)

if(SOS_IS_ARM)
	set(LIB_OPTION kernel)
endif()

sos_sdk_library_target(RELEASE ${PROJECT_NAME} "${LIB_OPTION}" release ${SOS_ARCH})

add_library(${RELEASE_TARGET} STATIC)

target_sources(${RELEASE_TARGET}
	PUBLIC
	${PUBLIC_SOURCES}
	PRIVATE
	${PRIVATE_SOURCELIST}
	)

target_compile_definitions(${RELEASE_TARGET}
	PUBLIC
	HAVE_CONFIG_H=1
	PRIVATE
	IS_LOCAL_BUILD=1
	)

target_compile_options(${RELEASE_TARGET}
	PUBLIC
	-Os
	)

target_include_directories(${RELEASE_TARGET}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/jansson/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	jansson/src
	)

sos_sdk_library_target(DEBUG ${PROJECT_NAME} "${LIB_OPTION}" debug ${SOS_ARCH})
add_library(${DEBUG_TARGET} STATIC)
sos_sdk_copy_target(${RELEASE_TARGET} ${DEBUG_TARGET})
sos_sdk_library_add_arch_targets("${DEBUG_OPTIONS}" ${SOS_ARCH} "")

if(SOS_IS_LINK)
	sos_sdk_library_target(COVERAGE ${PROJECT_NAME} "${LIB_OPTION}" coverage ${SOS_ARCH})
	add_library(${COVERAGE_TARGET} STATIC)
	sos_sdk_copy_target(${RELEASE_TARGET} ${COVERAGE_TARGET})
	sos_sdk_library_add_arch_targets("${COVERAGE_OPTIONS}" ${SOS_ARCH} "")
endif()

sos_sdk_library_add_arch_targets("${RELEASE_OPTIONS}" ${SOS_ARCH} "")

install(FILES ./jansson/src/jansson.h ./jansson_api.h ./jansson_config.h DESTINATION include/jansson)

install(FILES ${PROJECT_NAME}.cmake
	DESTINATION ${SOS_SDK_PATH}/cmake/targets)

cmake_minimum_required (VERSION 3.12)

set(GIT_REPOSITORY "https://github.com/akheron/jansson.git")
set(GIT_BRANCH "v2.14")
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/jansson)
	message(STATUS "Need to clone for the first call to cmake")
	execute_process(
		COMMAND git clone --depth 1 --branch ${GIT_BRANCH} ${GIT_REPOSITORY}
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
	jansson_api.c
)

set(PUBLIC_SOURCELIST
	${SOURCES_PREFIX}/jansson.h
	include/jansson/jansson_api.h
	include/jansson/jansson_config.h
)

if(CMSDK_IS_ARM)
	set(LIB_OPTION kernel)
endif()

cmsdk2_add_library(
	TARGET RELEASE_TARGET
	NAME ${PROJECT_NAME}
	OPTION "${LIB_OPTION}"
	CONFIG release
	ARCH ${CMSDK_ARCH}
)

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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/jansson>
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/jansson
	${CMAKE_CURRENT_SOURCE_DIR}
	jansson/src
	)

cmsdk2_add_library(
	TARGET DEBUG_TARGET
	NAME ${PROJECT_NAME}
	OPTION "${LIB_OPTION}"
	CONFIG debug
	ARCH ${CMSDK_ARCH}
)
cmsdk2_copy_target(
	SOURCE ${RELEASE_TARGET}
	DESTINATION ${DEBUG_TARGET}
)

cmsdk2_update_library_for_architecture(
	TARGET ${RELEASE_TARGET}
)

cmsdk2_update_library_for_architecture(
	TARGET ${DEBUG_TARGET}
)

install(FILES
	./jansson/src/jansson.h
	./include/jansson/jansson_api.h
	./include/jansson/jansson_config.h
	DESTINATION include/jansson)

install(FILES ${PROJECT_NAME}.cmake
	DESTINATION ${CMSDK_LOCAL_PATH}/cmake/targets)

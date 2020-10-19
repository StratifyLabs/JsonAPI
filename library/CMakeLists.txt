

sos_sdk_add_subdirectory(PRIVATE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src)
sos_sdk_add_subdirectory(INTERFACE_SOURCES include)

include(targets/StratifyAPI)

sos_sdk_library_target(RELEASE ${LOCAL_NAME} "" release ${SOS_ARCH})

add_library(${RELEASE_TARGET} STATIC)

set_property(TARGET ${RELEASE_TARGET} PROPERTY CXX_STANDARD 17)

target_sources(${RELEASE_TARGET}
	PRIVATE
	${INTERFACE_SOURCES}
	${PRIVATE_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/JsonAPI.cmake
	)

target_compile_definitions(${RELEASE_TARGET}
	PUBLIC
	PRIVATE
	)


target_include_directories(${RELEASE_TARGET}
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/${LOCAL_NAME}>
	PRIVATE
	)

set(LIBRARIES FsAPI ChronoAPI VarAPI)
if(SOS_IS_LINK)
	list(APPEND LIBRARIES jansson)
endif()

sos_sdk_library_target(DEBUG ${LOCAL_NAME} "${LIB_OPTION}" debug ${SOS_ARCH})
add_library(${DEBUG_TARGET} STATIC)
sos_sdk_copy_target(${RELEASE_TARGET} ${DEBUG_TARGET})
sos_sdk_library_add_arch_targets("${DEBUG_OPTIONS}" ${SOS_ARCH} "${LIBRARIES}")

if(SOS_IS_LINK)
	sos_sdk_library_target(COVERAGE ${LOCAL_NAME} "${LIB_OPTION}" coverage ${SOS_ARCH})
	add_library(${COVERAGE_TARGET} STATIC)
	sos_sdk_copy_target(${RELEASE_TARGET} ${COVERAGE_TARGET})
	sos_sdk_library_add_arch_targets("${COVERAGE_OPTIONS}" ${SOS_ARCH} "${LIBRARIES}")
endif()

sos_sdk_library_add_arch_targets("${RELEASE_OPTIONS}" ${SOS_ARCH} "${LIBRARIES}")

target_compile_options(${RELEASE_TARGET}
	PRIVATE
	-Os
	)

target_compile_options(${DEBUG_TARGET}
	PRIVATE
	-Os
	)

install(DIRECTORY include/
	DESTINATION include/${LOCAL_NAME}
	PATTERN CMakelists.txt EXCLUDE)

install(FILES JsonAPI.cmake
	DESTINATION cmake/targets)




set(LOCAL_NAME JsonAPI)

# This will set the default RAM used by the application
set(RAM_SIZE 32768)

set(DEPENDENCIES TestAPI FsAPI JsonAPI)
if(SOS_IS_LINK)
	#Stratify OS will have jansson include in the kernel
	list(APPEND DEPENDENCIES jansson)
endif()

sos_sdk_app_target(RELEASE ${LOCAL_NAME} "" release ${SOS_ARCH})
add_executable(${RELEASE_TARGET})
target_sources(${RELEASE_TARGET}
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/sl_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/UnitTest.hpp
	)

target_compile_options(${RELEASE_TARGET}
	PRIVATE
	-Os
	)

set_property(TARGET ${RELEASE_TARGET} PROPERTY CXX_STANDARD 17)



get_target_property(MY_DIR ${RELEASE_TARGET} BINARY_DIR)
message(STATUS "BINARY DIR for ${RELEASE_TARGET} is ${MY_DIR}")

include(CTest)

set(CTEST_OUTPUT_ON_FAILURE ON)
if(SOS_IS_LINK)

	add_test(NAME ${LOCAL_NAME}
		COMMAND "../build_release_link/${LOCAL_NAME}_link.elf" --api
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tmp
		)

	add_test(NAME coverage_${LOCAL_NAME}
		COMMAND "../build_coverage_link/${LOCAL_NAME}_coverage_link.elf" --api
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tmp
		)


	set_tests_properties(
		${LOCAL_NAME}
		PROPERTIES
		PASS_REGULAR_EXPRESSION "___finalResultPass___"
		)

	set_tests_properties(
		coverage_${LOCAL_NAME}
		PROPERTIES
		PASS_REGULAR_EXPRESSION "___finalResultPass___"
		)

	sos_sdk_app_target(COVERAGE ${LOCAL_NAME} "" coverage ${SOS_ARCH})
	add_executable(${COVERAGE_TARGET})
	sos_sdk_copy_target(${RELEASE_TARGET} ${COVERAGE_TARGET})

	set_target_properties(${COVERAGE_TARGET}
		PROPERTIES
		LINK_FLAGS --coverage)

endif()

sos_sdk_app_add_arch_targets("${RELEASE_OPTIONS}" "${DEPENDENCIES}" ${RAM_SIZE})
if(SOS_IS_LINK)
	sos_sdk_app_add_arch_targets("${COVERAGE_OPTIONS}" "${DEPENDENCIES}" ${RAM_SIZE})
endif()

target_compile_options(${RELEASE_TARGET}
	PRIVATE
	-Os
	)



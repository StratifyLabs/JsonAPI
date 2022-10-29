set(LIBRARIES FsAPI ChronoAPI VarAPI)
if(CMSDK_IS_LINK)
	list(APPEND LIBRARIES jansson)
endif()

cmsdk2_check_version(
	NAME FsAPI
	VERSION ${FsAPI_VERSION}
	MINIMUM_VERSION 1.6.4)

api2_add_library(
	NAME JsonAPI
	VERSION ${PROJECT_VERSION}
	DEPENDENCIES ${LIBRARIES}
	TARGETS TARGET_LIST)

if(CMSDK_IS_ARM AND API_IS_SDK)
	foreach(TARGET ${TARGET_LIST})
		target_include_directories(${TARGET}
			PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/include/jansson>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/jansson/src>)
	endforeach()
endif()

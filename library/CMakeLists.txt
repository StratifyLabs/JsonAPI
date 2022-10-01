set(LIBRARIES FsAPI ChronoAPI VarAPI)
if(CMSDK_IS_LINK)
	list(APPEND LIBRARIES jansson)
endif()

api2_add_library(
	NAME JsonAPI
	DEPENDENCIES ${LIBRARIES}
)

if(CMSDK_IS_ARM AND API_IS_SDK)
	foreach(TARGET ${TARGET_LIST})
		target_include_directories(${TARGET}
			PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/include/jansson>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/jansson/src>
			PRIVATE
			)
	endforeach()
endif()

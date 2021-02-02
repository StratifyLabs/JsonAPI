
set(LIBRARIES FsAPI ChronoAPI VarAPI)
if(SOS_IS_LINK)
	list(APPEND LIBRARIES jansson)
endif()

api_add_api_library(JsonAPI "${LIBRARIES}")

if(SOS_IS_ARM AND API_IS_SDK)
	macro(add_jansson_include ARCH)
		target_include_directories(JsonAPI_debug_${ARCH}
			PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/jansson/src>
			PRIVATE
			)

		target_include_directories(JsonAPI_release_${ARCH}
			PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../jansson/jansson/src>
			PRIVATE
			)
	endmacro()

	add_jansson_include(${SOS_ARCH})
	foreach(ARCH ${SOS_ARCH_LIST})
		add_jansson_include(${ARCH})
	endforeach()
endif()

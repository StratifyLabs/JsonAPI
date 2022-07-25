
if(NOT DEFINED API_IS_SDK)
	include(API)
	if(CMSDK_IS_LINK)
		cmsdk_include_target(jansson "${API_CONFIG_LIST}")
	endif()
	cmsdk_include_target(JsonAPI "${API_CONFIG_LIST}")
endif()

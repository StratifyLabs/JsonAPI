
if(NOT DEFINED IS_SDK)
	include(API)
	if(SOS_IS_LINK)
		sos_sdk_include_target(jansson "${STRATIFYAPI_CONFIG_LIST}")
	endif()
	sos_sdk_include_target(JsonAPI "${STRATIFYAPI_CONFIG_LIST}")
endif()

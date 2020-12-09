
if(NOT DEFINED IS_SDK AND SOS_IS_ARM)
	sos_sdk_include_target(jansson_kernel "${API_CONFIG_LIST}")
endif()

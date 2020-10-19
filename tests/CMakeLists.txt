

set(DEPENDENCIES TestAPI FsAPI JsonAPI)
if(SOS_IS_LINK)
	#Stratify OS will have jansson include in the kernel
	list(APPEND DEPENDENCIES jansson)
endif()

stratify_api_add_test_executable(JsonAPI 32768 "${DEPENDENCIES}")




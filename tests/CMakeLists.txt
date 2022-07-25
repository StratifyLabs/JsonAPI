

set(DEPENDENCIES JsonAPI TestAPI FsAPI)
if(CMSDK_IS_LINK)
	#Stratify OS will have jansson include in the kernel
	list(APPEND DEPENDENCIES jansson)
endif()

api_add_test_executable(JsonAPI 32768 "${DEPENDENCIES}")




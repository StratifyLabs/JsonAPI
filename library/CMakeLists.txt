
set(LIBRARIES FsAPI ChronoAPI VarAPI)
if(SOS_IS_LINK)
	list(APPEND LIBRARIES jansson)
endif()


stratifyapi_add_api_library(JsonAPI "${LIBRARIES}")




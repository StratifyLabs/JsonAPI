cmake_minimum_required (VERSION 3.12)
cmsdk2_minimum_required(2.1.2)
project(JsonAPI
	LANGUAGES CXX C
	VERSION 1.5.2)

add_subdirectory(jansson)
add_subdirectory(library)

option(JSON_API_IS_TEST "Enable test builds for JsonAPI" OFF)
if(JSON_API_IS_TEST)
	add_subdirectory(tests)
endif()


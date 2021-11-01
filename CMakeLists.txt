cmake_minimum_required (VERSION 3.12)

project(JsonAPI CXX)
include(CTest)

add_subdirectory(jansson jansson)
add_subdirectory(library library)

option(JSON_API_IS_TEST "Enable test builds for JsonAPI" OFF)
if(JSON_API_IS_TEST)
	add_subdirectory(tests tests)
endif()


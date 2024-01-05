cmake_minimum_required(VERSION 3.19)

project(CMakeGo 
	VERSION 0.1.0
	DESCRIPTION ""
	LANGUAGES C CXX)

find_package(spdlog CONFIG REQUIRED)

add_executable(CMakeGo src/main.cpp src/logger.cpp src/cmake.cpp)

target_include_directories(CMakeGo PRIVATE vendor)

set_target_properties(CMakeGo
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(CMakeGo 
		PRIVATE
		spdlog::spdlog)	

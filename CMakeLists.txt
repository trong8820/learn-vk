cmake_minimum_required(VERSION 3.16)

project ("learn-vk" VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_UNITY_BUILD ON)

# Detect platform
#WINDOWS   	=  Windows
#ANDROID    =  Android
#LINUX      =  Linux
#MACOS    	=  macOS
#IOS    	=  iOS
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(WINDOWS TRUE)
	set(PLATFORM_FOLDER windows)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	set(ANDROID TRUE)
	set(PLATFORM_FOLDER android)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(ANDROID)
		set(ANDROID TRUE)
		set(PLATFORM_FOLDER android)
	else()
		set(LINUX TRUE)
		set(PLATFORM_FOLDER linux)
	endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(APPLE TRUE)
	set(MACOS TRUE)
	set(PLATFORM_FOLDER macos)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
	set(APPLE TRUE)
	set(IOS TRUE)
	set(PLATFORM_FOLDER ios)
else()
	message(FATAL_ERROR "Unsupported platform, CMake will exit")
	return()
endif()

# 3rdparty include
include_directories (
	${CMAKE_SOURCE_DIR}/3rdparty/glm-0.9.9.8/include
	${CMAKE_SOURCE_DIR}/3rdparty/glfw-3.3.4/include
	${CMAKE_SOURCE_DIR}/3rdparty/vulkan-1.2.182/include
)

# 3rdparty sources
#file(GLOB_RECURSE 3RDPARTY_SOURCE_FILES
#	${CMAKE_SOURCE_DIR}/3rdparty/*.c
#	${CMAKE_SOURCE_DIR}/3rdparty/*.cc
#	${CMAKE_SOURCE_DIR}/3rdparty/*.cpp
#)

# 3rdparty libs
link_directories(
	${CMAKE_SOURCE_DIR}/3rdparty/glfw-3.3.4/lib/${PLATFORM_FOLDER}
	${CMAKE_SOURCE_DIR}/3rdparty/vulkan-1.2.182/lib/${PLATFORM_FOLDER}
)

if (WINDOWS)
link_libraries(glfw3 vulkan-1)
else()
link_libraries(glfw3 vulkan)
endif()

if (APPLE)
	link_libraries(
		"-framework AppKit"
		"-framework IOKit"
		"-framework CoreGraphics"
	)
endif()

# Projects
include_directories(
	 ${CMAKE_SOURCE_DIR}/include
)

# add_executable(${PROJECT_NAME} ${3RDPARTY_SOURCE_FILES} ${CMAKE_SOURCE_DIR}/src/main.cpp)
# add_executable(${PROJECT_NAME} ${3RDPARTY_SOURCE_FILES} ${CMAKE_SOURCE_DIR}/src/main01.cpp)
add_executable(${PROJECT_NAME} ${3RDPARTY_SOURCE_FILES} ${CMAKE_SOURCE_DIR}/src/main02.cpp)

add_custom_command(TARGET  ${PROJECT_NAME} PRE_BUILD
				   COMMAND "python" ${CMAKE_SOURCE_DIR}/shaders/compile.py
				   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/shaders
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)
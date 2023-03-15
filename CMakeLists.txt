cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project (geoff VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (Boost_USE_STATIC_LIBS ON)

if(MSVC)
  set (Boost_USE_STATIC_RUNTIME OFF)
  add_definitions(-D_WIN32_WINNT=0x0601)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS=1)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
  add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
  add_compile_options(/MP)
  string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)

find_package(OpenSSL REQUIRED)

cmake_policy(SET CMP0074 NEW)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE BEAST_FILES
  ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
  ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)

source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

file(GLOB APP_SRC_FILES
	src/geoff.cpp
	src/SSLContext.cpp)

file(GLOB APP_INC_FILES
	src/SSLContext.h)

source_group(src FILES ${APP_SRC_FILES})
source_group(inc FILES ${APP_INC_FILES})

add_executable(geoff ${APP_SRC_FILES} ${APP_INC_FILES} ${BEAST_FILES})

if(NOT WIN32)
  find_package(Threads)
  target_link_libraries(geoff PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY})
  target_link_libraries(geoff PRIVATE OpenSSL::SSL)
endif()

set_target_properties(geoff PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT geoff)


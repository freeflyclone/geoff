cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project (geoff VERSION 0.1.0 LANGUAGES CXX)
include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(Boost_NO_WARN_NEW_VERSIONS 1)

set (Boost_USE_STATIC_LIBS ON)
set (OPENSSL_USE_STATIC_LIBS TRUE)
set (OPENSSL_MSVC_STATIC_RT TRUE)

if(MSVC)
  set (Boost_USE_STATIC_RUNTIME OFF)
  add_definitions(-D_WIN32_WINNT=0x0601)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS=1)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
  add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
  add_compile_options(/MP /bigobj)
  string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)

cmake_policy(SET CMP0074 NEW)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE BEAST_FILES
  ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
  ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)

message(STATUS "Boost: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost: ${Boost_LIBRARY_DIRS}")
message(STATUS "OpenSSL: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL: ${OPENSSL_LIBRARIES}")
message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")

source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

file(GLOB APP_SRC_FILES
	src/geoff.cpp
	src/SSLContext.cpp)

file(GLOB APP_INC_FILES
	src/SSLContext.h)

if(MSVC)
  source_group("Source Files" FILES ${APP_SRC_FILES})
  source_group("Header Files" FILES ${APP_INC_FILES})
else()
  source_group(src FILES ${APP_SRC_FILES})
  source_group(inc FILES ${APP_INC_FILES})
endif()

add_executable(geoff ${APP_SRC_FILES} ${APP_INC_FILES} ${BEAST_FILES})

find_package(Threads)
target_link_libraries(geoff PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY})
target_link_libraries(geoff PRIVATE OpenSSL::SSL)

if(MSVC)
  target_link_libraries(geoff PRIVATE Crypt32)
endif()

set_target_properties(geoff PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT geoff)


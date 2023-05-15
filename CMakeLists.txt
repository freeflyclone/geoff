# CMake project file for GEOFF
# Adding files to the project should be done in this file, to preserve cross-platform building
#
# Note: proper functioning of this file depends on two environment variables:
#
#  BOOST_ROOT:			set to the root folder of the installed Boost library
#						MSVC example: D:\DevLibs\boost_1_81_0
#
#  OPENSSL_ROOT_DIR:	set to root folder of installed OpenSSL libraries
#						MSVC example: D:\DevLibs\OpenSSL-WIN64  
#---------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project (geoff VERSION 0.1.0 LANGUAGES CXX)

set(GEOFF_LIB_NAME GeoffLib)
set(GEOFF_APP_NAME Asteroids)

# set this to ON to enable the message() calls below
# that display various cmake path variable evaluations
set(CMAKE_PROJECT_DEBUG OFF)

# This is MSVC specific
set(CMAKE_CONFIGURATION_TYPES Debug;Release)

# These are relative to ${PROJECT_SOURCE_DIR}, that is, where it was cloned to.
# Files you want to appear in the "Source Files" project folder are explicitly listed here
file(GLOB GEOFF_APP_SRC_FILES
	apps/asteroids/src/Asteroids.cpp
	apps/asteroids/src/AsteroidsMain.cpp
	apps/asteroids/src/AsteroidsSession.cpp
	apps/asteroids/src/Bullet.cpp
	apps/asteroids/src/Gun.cpp
	apps/asteroids/src/Player.cpp
	apps/asteroids/src/Rock.cpp
	apps/asteroids/src/RockField.cpp
	apps/asteroids/src/Session.cpp
	apps/asteroids/src/Ship.cpp
	apps/asteroids/src/Timer.cpp
	apps/asteroids/src/Universe.cpp)
	
# Files to appear in "Header Files" project folder are explicitly listed here
file(GLOB GEOFF_APP_INC_FILES
	apps/asteroids/inc/Asteroids.h
	apps/asteroids/inc/AsteroidsSession.h
	apps/asteroids/inc/Bullet.h
	apps/asteroids/inc/Gun.h
	apps/asteroids/inc/Player.h
	apps/asteroids/inc/Rock.h
	apps/asteroids/inc/RockField.h
	apps/asteroids/inc/Session.h
	apps/asteroids/inc/Ship.h
	apps/asteroids/inc/Timer.h
	apps/asteroids/inc/Universe.h)
	
file(GLOB GEOFF_LIB_SRC_FILES
	lib/src/AppBuffer.cpp
	lib/src/GameSession.cpp
	lib/src/geoff.cpp
	lib/src/SSLContext.cpp
	lib/src/ServerUtils.cpp
	lib/src/WebsockServer.cpp
	lib/src/WebsockSession.cpp)

file(GLOB GEOFF_LIB_INC_FILES
	apps/AppSession.h
	lib/inc/AppBuffer.h
	lib/inc/GameSession.h
	lib/inc/geoff.h
    lib/inc/handle_request.h
	lib/inc/server_certificate.hpp
	lib/inc/ServerUtils.h
	lib/inc/SSLContext.h
	lib/inc/websocket_session.h
	lib/inc/WebsockServer.h
	lib/inc/WebsockSession.h
	lib/inc/WebsockSessionManager.h)

# Files within the source tree "www" tree are all implicitly (automatically) added.
# Things like .html, .js, .gif, etc. The tree structure is preserved. 
file(GLOB_RECURSE ASTEROIDS_WWW_FILES
	www/asteroids/*)

# I believe boost wants these specifically, I'm not sure geoff code cares otherwise. (yet)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_NO_WARN_NEW_VERSIONS 1)
set (Boost_USE_STATIC_LIBS ON)

set (OPENSSL_USE_STATIC_LIBS TRUE)
set (OPENSSL_MSVC_STATIC_RT TRUE)

# Compiler & Build flags
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

# USE_FOLDERS is IDE semantic: enables project file folders, rather than filesystem folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# where targets go. default if not set: CMakePredefinedTargets is used
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)

cmake_policy(SET CMP0074 NEW)

# need OpenSSL for https: & wss:
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

# need boost/beast & boost/asio
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# get list of boost/beast files to project.
file(GLOB_RECURSE BEAST_FILES
  ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
  ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)

if(CMAKE_PROJECT_DEBUG)
  message(STATUS "Project name: ${PROJECT_NAME}")
  message(STATUS "Boost: ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost: ${Boost_LIBRARY_DIRS}")
  message(STATUS "OpenSSL: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL: ${OPENSSL_LIBRARIES}")
  message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
endif()

# explicitly add boost/beast files to project files.
# WARNING: tweeking this within Visual Studio is ill-advised
source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

if(MSVC)
	source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${ASTEROIDS_WWW_FILES})
endif()

add_executable(${GEOFF_APP_NAME} ${GEOFF_APP_SRC_FILES} ${GEOFF_APP_INC_FILES} ${ASTEROIDS_WWW_FILES})
add_library(${GEOFF_LIB_NAME} STATIC ${GEOFF_LIB_SRC_FILES} ${GEOFF_LIB_INC_FILES})

find_package(Threads)

target_include_directories(${GEOFF_LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib/inc ${PROJECT_SOURCE_DIR}/apps)
target_include_directories(${GEOFF_APP_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib/inc ${PROJECT_SOURCE_DIR}/apps ${PROJECT_SOURCE_DIR}/apps/asteroids/inc)

# if GeoffLib is ever generated as a shared library, we want IT to be dependent on these.
# (Note: doesn't seem to make any difference in VC when built as static library, which jives with what I know about linking)
target_link_libraries(${GEOFF_LIB_NAME} PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY})
target_link_libraries(${GEOFF_LIB_NAME} PRIVATE OpenSSL::SSL)

target_link_libraries(${GEOFF_APP_NAME} PRIVATE ${GEOFF_LIB_NAME})

if(MSVC)
  target_link_libraries(${GEOFF_APP_NAME} PRIVATE Crypt32)

  set_target_properties(${GEOFF_APP_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  set_target_properties(${GEOFF_APP_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "0.0.0.0 8080 www 4")
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${GEOFF_APP_NAME})
endif()


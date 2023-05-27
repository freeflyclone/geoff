# GEoFF - Games Everyone Finds Fun.

## Introduction
GEoFF a project to provide a custom lightweight HTML and WebSocket server, built using the powerful Boost::beast library.

It's inspired by [Vinnie Falco “Get rich quick! Using Boost.Beast WebSockets and Networking TS”](https://www.youtube.com/watch?v=7FQwAjELMek), because who doesn't want to get rich quick?  I know I do. 

WebSockets provide session based, 2-way, real-time communication between a web client 
and server.  The Boost::beast libary is well suited to provide a very lightweight 
multiplayer game server for HTML 5 games written in JavaScript that will accomodate 
large numbers of players simultaneously.  GEoFF is NOT another Apache, IIS, or nginx.
It's designed to support multiplayer web games, and nothing else.

## Dependencies
On the author's Windows 10 development machine, these get installed to D:\DevLibs...
- Boost - Download from [here](https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.7z)
    - Read the included  **INSTALL** file
    - Set environment variable **BOOST_ROOT=D:\DevLibs\boost_1_81_0**

- OpenSSL - Download from [here](https://slproweb.com/download/Win64OpenSSL-1_1_1t.msi)
    - Launch the installer file,  Use **D:\DevLibs\OpenSSL-Win64** as the install folder, default choices for all else.
    - Set environment variable **OPENSSL_ROOT_DIR=D:\DevLibs\OpenSSL-Win64**

## CMake build system
To facilitate cross-platform development and/or deployment, the C++ parts are built with CMake.
### Windows
 - Consider installing [CMake for Windows](https://github.com/Kitware/CMake/releases/download/v3.26.1/cmake-3.26.1-windows-x86_64.msi).  
 - Clone this repository to your local machine, in a folder of your choosing.
     - eg: D:\src\geoff
 - Make a build folder for CMake to create the project files
     - eg: D:\src\geoff\build
 - Run *CMake (cmake-gui)* from the Windows Start menu
   - Set the *Where is the source code:* field to 'D:/src/geoff'
   - Set the *Where to build the binaries:* field to 'D:/src/geoff/build'
 - Click the *Configure* button, then the *Generate* button.
 - CMake will create a geoff.sln file in D:\src\geoff\build.

cmake_minimum_required (VERSION 3.1)

project (blescanner_project)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -pedantic -g")

add_executable(blescanner
	blescanner.cpp
	HciDev.cpp
	Socket.cpp
	HciRequest.cpp
	MainLoop.cpp
	HciSocket.cpp
	BLEAddress.cpp
	BLEDevice.cpp
	L2CAPSocket.cpp
	BLEGATT.cpp
	UUID.cpp
	ByteBuffer.cpp
	utils.cpp
		)


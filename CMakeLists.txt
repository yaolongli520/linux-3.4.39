cmake_minimum_required (VERSION 3.5)

project (ctrl_app)

include_directories (${PROJECT_SOURCE_DIR}/include)

set (SRC_SERVER
	${PROJECT_SOURCE_DIR}/src/core/main.cpp
	${PROJECT_SOURCE_DIR}/src/time/mytime.cpp
	${PROJECT_SOURCE_DIR}/src/disk/disk.cpp  
	${PROJECT_SOURCE_DIR}/src/io_ctrl/ioctrl.cpp  
	${PROJECT_SOURCE_DIR}/src/lcd/lcd.cpp  
	${PROJECT_SOURCE_DIR}/src/human/human.cpp  
	${PROJECT_SOURCE_DIR}/src/humiture/humiture.cpp  
	)

add_executable(ctrl_server ${SRC_SERVER})
target_link_libraries (ctrl_server pthread)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

SET(CMAKE_C_COMPILER "/opt/FriendlyARM/toolchain/4.9.3/bin/arm-linux-gcc")
SET(CMAKE_CXX_COMPILER "/opt/FriendlyARM/toolchain/4.9.3/bin/arm-linux-g++")


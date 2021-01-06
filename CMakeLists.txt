cmake_minimum_required (VERSION 3.5)

project (ctrl_app)

include_directories (${PROJECT_SOURCE_DIR}/include)
include_directories (${PROJECT_SOURCE_DIR}/include/freetype2)
include_directories (${PROJECT_SOURCE_DIR}/include/freetype2/freetype)


set (SRC_SERVER
	${PROJECT_SOURCE_DIR}/src/core/main.cpp
	${PROJECT_SOURCE_DIR}/src/time/mytime.cpp
	${PROJECT_SOURCE_DIR}/src/disk/disk.cpp  
	${PROJECT_SOURCE_DIR}/src/io_ctrl/ioctrl.cpp  
	${PROJECT_SOURCE_DIR}/src/lcd/lcd.cpp  
	${PROJECT_SOURCE_DIR}/src/lcd/picture.cpp  
	${PROJECT_SOURCE_DIR}/src/lcd/characters.cpp  
	${PROJECT_SOURCE_DIR}/src/human/human.cpp  
	${PROJECT_SOURCE_DIR}/src/humiture/humiture.cpp  
	${PROJECT_SOURCE_DIR}/src/nrf24l01/nrf24l01.cpp  
	${PROJECT_SOURCE_DIR}/src/net/wireless.cpp  
	${PROJECT_SOURCE_DIR}/src/pthread/pth_net_server.cpp
	${PROJECT_SOURCE_DIR}/src/pthread/pth_nrf_server.cpp
	${PROJECT_SOURCE_DIR}/src/pthread/pth_show.cpp
	)

#指定freetype库路径
find_library(FREETYPE_LIB freetype HINTS ${PROJECT_SOURCE_DIR}/src/lcd/libfreetype)

add_executable(ctrl_server ${SRC_SERVER})
target_link_libraries (ctrl_server pthread)
target_link_libraries (ctrl_server jpeg)
target_link_libraries (ctrl_server ${FREETYPE_LIB})


#set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/src/lcd/libjpeg)
#set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/src/lcd/libfreetype)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

SET(CMAKE_C_COMPILER "/opt/FriendlyARM/toolchain/4.9.3/bin/arm-linux-gcc")
SET(CMAKE_CXX_COMPILER "/opt/FriendlyARM/toolchain/4.9.3/bin/arm-linux-g++")


project(2016-Vision)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

find_package(OpenCV REQUIRED)
find_package(Freenect REQUIRED)
find_package(libusb-1.0 REQUIRED)

add_executable(2016-Vision ninjaVision.cpp cvFreenect.cpp)
target_include_directories(2016-Vision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${LIBUSB_1_INCLUDE_DIRS} ${FREENECT_INCLUDE_DIR} ${OpenCV2_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/ntcore/include")
target_link_libraries(2016-Vision LINK_PUBLIC ${OpenCV_LIBS} ${FREENECT_freenect_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} "${CMAKE_CURRENT_SOURCE_DIR}/ntcore/libntcore.so")

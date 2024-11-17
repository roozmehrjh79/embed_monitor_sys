cmake_minimum_required(VERSION 3.1)
project(WebcamMonitor)

# Flags & Config
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sources
set(APP_SOURCES src/WebcamMonitor.cpp)
set(OpenCV_DIR ~/Documents/OpenCV/installation/OpenCV-master/lib/cmake/opencv4)

# Includes & Libraries
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories("/usr/include/mariadb/;/usr/include/mariadb/mysql/;/usr/include/alsa/")
link_directories("/usr/local/lib/mysql/;/usr/lib/x86_64-linux-gnu/")

# Executables & Links
add_executable(${PROJECT_NAME} ${APP_SOURCES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} mysqlclient mariadb pthread)

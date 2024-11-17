cmake_minimum_required(VERSION 3.3)
project(httpServer)

# Flags & Config
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF)

# Sources
set(APP_SOURCES src/httpServer.cpp)
set(MISC_SOURCES src/fields_alloc.hpp src/Jamfile)
set(CMAKE_PREFIX_PATH /home/roozmehr/Documents/boost_1_79_0)
set(Boost_INCLUDE_DIR ~/Documents/boost_1_79_0)

# Includes & Libraries
#find_package(Boost 1.79 COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR} "/usr/include/mariadb/;/usr/include/mariadb/mysql/;/usr/include/alsa/")
link_directories("/usr/local/lib/mysql/;/usr/lib/x86_64-linux-gnu/")

# Executables & Links
add_executable(${PROJECT_NAME} ${APP_SOURCES} ${MISC_SOURCES} ${BOOST_BEAST_FILES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} mysqlclient mariadb pthread)

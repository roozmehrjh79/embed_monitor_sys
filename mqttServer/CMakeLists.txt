cmake_minimum_required(VERSION 3.1)
project(MQTT_Server)

# Flags & Config
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Sources
set(APP_SOURCES src/mqtt_server.c)

# Includes & Libraries
include_directories("/usr/include/mariadb/;/usr/include/mariadb/mysql/")
link_directories("/usr/local/lib/mysql/;/usr/lib/x86_64-linux-gnu/")

# Executables & Links
add_executable(${PROJECT_NAME} ${APP_SOURCES})
target_link_libraries(${PROJECT_NAME} mysqlclient mariadb paho-mqtt3c)

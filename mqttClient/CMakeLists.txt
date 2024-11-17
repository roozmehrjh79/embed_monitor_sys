cmake_minimum_required(VERSION 3.1)
project(MQTT_Client)

# Flags & Config
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Sources
set(APP_SOURCES src/mqtt_client.c)

# Executables & Links
add_executable(${PROJECT_NAME} ${APP_SOURCES})
target_link_libraries(${PROJECT_NAME} paho-mqtt3c)

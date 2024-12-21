cmake_minimum_required(VERSION 3.10)
project(sensor_gateway C)             # Tên dự án và khai báo ngôn ngữ là C

# Thiết lập tiêu chuẩn của C (ví dụ: C11 hoặc C99)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(inc)
include_directories(lib/inc)


add_library(sighand STATIC src/sighand.c)
add_library(sensorlib STATIC lib/src/sensor.c)

add_executable(sensor_gateway src/server.c)
add_executable(tem_sensor src/client.c)

target_link_libraries(sensor_gateway sighand)
target_link_libraries(tem_sensor sensorlib)

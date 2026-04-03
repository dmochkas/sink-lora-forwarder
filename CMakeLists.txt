cmake_minimum_required(VERSION 3.10)
project(sink-lora-forwarder C)

include(config.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(AHOI_SERIAL_LIB "ahoi-serial-lib::ahoi-serial-lib")
set(ZLOG_LIB "zlog::zlog")

find_package(ahoi-serial-lib REQUIRED)
find_package(zlog REQUIRED)

add_subdirectory(src)

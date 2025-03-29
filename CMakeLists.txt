cmake_minimum_required(VERSION 3.10)
project(vpnclient_driver)

add_library(vpnclient_driver STATIC
    src/core/driver.c
    src/core/socks5.c
)

if(WIN32)
    add_library(platform STATIC src/platform/windows/tap_device.c)
#elseif(ANDROID)
#    add_library(platform STATIC src/platform/android/tun_device.jni)
elseif(APPLE)
    add_library(platform STATIC src/platform/macos/tun_device.c)
else()
    add_library(platform STATIC src/platform/linux/tun_device.c)
endif()

target_include_directories(vpnclient_driver PUBLIC include)
target_link_libraries(vpnclient_driver PRIVATE platform)
cmake_minimum_required(VERSION 3.10)
project(VPNClientEngineLinux)

set(CMAKE_C_STANDARD 11)

# Настройка OpenSSL
find_package(OpenSSL REQUIRED)

# Основная библиотека
add_library(vpnclient_engine_linux STATIC
    src/vpnclient_engine_linux.c
#    src/tun_device.c
#    src/crypto.c
)

target_include_directories(vpnclient_engine_linux PUBLIC include)
target_link_libraries(vpnclient_engine_linux OpenSSL::SSL OpenSSL::Crypto)

# Тесты
add_executable(test_client test/test_vpnclient_engine_linux.c)
target_link_libraries(test_client vpnclient_engine_linux)

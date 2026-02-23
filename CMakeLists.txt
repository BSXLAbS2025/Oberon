cmake_minimum_required(VERSION 3.10)
project(Oberon C)

# Стандарт C99
set(CMAKE_C_STANDARD 99)

# Исходный файл
add_executable(oberon oberon.c)

# Подключаем библиотеку сокетов для Windows
if(WIN32)
    target_link_libraries(oberon ws2_32)
endif()

# Опции для macOS (Universal Binary)
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

cmake_minimum_required(VERSION 3.10)
project(Oberon C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Лечим проблему v3.3: Принудительно задаем версию через макрос
add_definitions(-DOBERON_VERSION="v4.0-Patch2")

include_directories(include)

# Собираем все .c файлы в src и src/modules (встроенные модули)
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(oberon ${SOURCES})

if(WIN32)
    target_link_libraries(oberon ws2_32)
    # Позволяет ядру экспортировать символы для плагинов
    set_target_properties(oberon PROPERTIES ENABLE_EXPORTS TRUE)
else()
    find_package(Threads REQUIRED)
    # CMAKE_DL_LIBS линкует libdl для работы dlopen/dlsym
    target_link_libraries(oberon Threads::Threads ${CMAKE_DL_LIBS})
endif()

# Создаем папку modules в директории сборки, если её нет
add_custom_command(TARGET oberon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:oberon>/modules
)

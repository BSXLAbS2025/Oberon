cmake_minimum_required(VERSION 3.10)
project(Oberon C)

# Устанавливаем стандарт C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Чтобы CMake не подсовывал старые объектные файлы (лечим v3.3)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# Пути к заголовкам
include_directories(include)

# Собираем ТОЛЬКО ядро (все .c в корне src, кроме папки modules)
file(GLOB CORE_SOURCES "src/*.c")
add_executable(oberon ${CORE_SOURCES})

# Настройка библиотек для Ядра
if(WIN32)
    target_link_libraries(oberon ws2_32)
    # Для Windows разрешаем экспорт функций, если нужно
    set_target_properties(oberon PROPERTIES ENABLE_EXPORTS TRUE)
else()
    # Для Linux/Mac/Termux добавляем потоки и библиотеку для загрузки плагинов (dl)
    find_package(Threads REQUIRED)
    target_link_libraries(oberon Threads::Threads ${CMAKE_DL_LIBS})
endif()

# --- СЕКЦИЯ МОДУЛЕЙ (Плагины) ---
# Если хочешь собрать тестовый модуль сразу, раскомментируй строки ниже:
# add_library(web_ui MODULE src/modules/web_ui.c)
# set_target_properties(web_ui PROPERTIES PREFIX "" SUFFIX ".so") # или .dll на Win

# Инструкция по установке
install(TARGETS oberon DESTINATION bin)

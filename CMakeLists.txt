cmake_minimum_required(VERSION 3.15)
project(zmq-client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к локальным библиотекам
set(LOCAL_LIB_DIR ${CMAKE_SOURCE_DIR}/lib)
set(LOCAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Установка выходного каталога для бинарников
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Устанавливаем выходной каталог для исполняемых файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 1. Ручной поиск ZeroMQ
find_library(ZMQ_LIBRARY
        NAMES zmq libzmq
        PATHS ${LOCAL_LIB_DIR}
        NO_DEFAULT_PATH
        REQUIRED
)

find_path(ZMQ_INCLUDE_DIR
        NAMES zmq.h
        PATHS ${LOCAL_INCLUDE_DIR}
        NO_DEFAULT_PATH
        REQUIRED
)

message(STATUS "Found ZeroMQ:")
message(STATUS "  - Includes: ${ZMQ_INCLUDE_DIR}")
message(STATUS "  - Library: ${ZMQ_LIBRARY}")

# 2. Создание исполняемого файла
add_executable(zmq-client src/test_client.cpp)

# 3. Настройка include директорий
target_include_directories(zmq-client PRIVATE
        ${ZMQ_INCLUDE_DIR}
        ${LOCAL_INCLUDE_DIR}
        ${LOCAL_INCLUDE_DIR}/cppzmq  # Если используется cppzmq
)

# 4. Линковка
target_link_libraries(zmq-client PRIVATE
        ${ZMQ_LIBRARY}
        pthread
)

# 5. Копирование .so файла в каталог с бинарником
add_custom_command(TARGET zmq-client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ZMQ_LIBRARY}
        $<TARGET_FILE_DIR:zmq-client>
)

# ================= WINDOWS ===================
#cmake_minimum_required(VERSION 3.15)
#project(zmq-client)
#
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
## Указываем базовый путь к MinGW
#set(MINGW_ROOT "C:/msys64/mingw64")
#set(PROJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#set(EXECUTABLE_OUTPUT_PATH ${PROJ_DIR}/bin)
#
#include(${PROJ_DIR}/.cmake/find_tools.cmake)
#
## 1. Ручная настройка путей ZeroMQ
#set(ZMQ_ROOT_DIR ${MINGW_ROOT})  # Базовый путь
#set(ZMQ_INCLUDE_DIR "${MINGW_ROOT}/include")
#set(ZMQ_LIBRARY "${MINGW_ROOT}/lib/libzmq.dll.a")
#
##set(ZMQ_ROOT_DIR "${CMAKE_SOURCE_DIR}/.local")  # Базовый путь
##set(ZMQ_INCLUDE_DIR "${ZMQ_ROOT_DIR}/include")  # Заголовки
##set(ZMQ_LIB_DIR "${ZMQ_ROOT_DIR}/lib64")        # Библиотеки
#
## 2. Поиск библиотеки вручную
##find_library(ZMQ_LIBRARY
##        NAMES zmq libzmq
##        PATHS ${ZMQ_LIB_DIR}
##        NO_DEFAULT_PATH
##        REQUIRED
##)
##
##find_path(ZMQ_INCLUDE_DIR
##        NAMES zmq.h
##        PATHS ${ZMQ_INCLUDE_DIR}
##        NO_DEFAULT_PATH
##        REQUIRED
##)
#
## Проверка существования файлов
#if(NOT EXISTS "${ZMQ_INCLUDE_DIR}/zmq.h")
#    message(FATAL_ERROR "ZeroMQ headers not found in ${ZMQ_INCLUDE_DIR}")
#endif()
#if(NOT EXISTS "${ZMQ_LIBRARY}")
#    message(FATAL_ERROR "ZeroMQ library not found: ${ZMQ_LIBRARY}")
#endif()
#
## 3. Настройка цели
#message(STATUS "Found ZeroMQ:")
#message(STATUS "  - Includes: ${ZMQ_INCLUDE_DIR}")
#message(STATUS "  - Library: ${ZMQ_LIBRARY}")
#
## 4. Подключение nlohmann_json (если тоже нужно локально)
##set(JSON_ROOT_DIR "${CMAKE_SOURCE_DIR}")
##set(JSON_INCLUDE_DIR "${JSON_ROOT_DIR}/include/nlohmann")
##find_package(nlohmann_json REQUIRED)
#
## 5. Добавляем цель
#add_executable(zmq-client src/test_client.cpp)
#
## 6. Настройка include директорий
#target_include_directories(zmq-client
#        PRIVATE
#        ${ZMQ_INCLUDE_DIR}
#        ${CMAKE_SOURCE_DIR}/include
#        ${CMAKE_SOURCE_DIR}/include/cppzmq
#)
#
## 7. Линкуем библиотеки
#target_link_libraries(zmq-client PRIVATE
#        ${ZMQ_LIBRARY}
#        winpthread
#        pthread  # ZeroMQ требует pthread
##        ws2_32
##        iphlpapi
##        secur32
#)

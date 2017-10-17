CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(alarma2-esp)

SET(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)

INCLUDE_DIRECTORIES($ENV{IDF_PATH}/components/lwip/include/lwip)
INCLUDE_DIRECTORIES($ENV{IDF_PATH}/components/esp32/include)
INCLUDE_DIRECTORIES($ENV{IDF_PATH}/components/log/include)
INCLUDE_DIRECTORIES($ENV{IDF_PATH}/components/driver/include)
INCLUDE_DIRECTORIES($ENV{IDF_PATH}/components/nvs_flash/include)
INCLUDE_DIRECTORIES($ENV{IDF_PATH}/components/freertos/include)
INCLUDE_DIRECTORIES($ENV{IDF_PATH}/components/btstack/src)

FILE(GLOB FILES src/*.h src/*.c)
ADD_EXECUTABLE(alarma2-esp
    ${FILES}
)

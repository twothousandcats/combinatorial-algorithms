cmake_minimum_required(VERSION 4.0)
project(qap)

set(CMAKE_CXX_STANDARD 20)

add_executable(qap main.cpp)

# копирование тестового окружения и входного файла
configure_file(test.sh ${CMAKE_CURRENT_BINARY_DIR}/test.sh COPYONLY)
configure_file(input.in ${CMAKE_CURRENT_BINARY_DIR}/input.in COPYONLY)
file(COPY tests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Установка прав на выполнение для .sh
execute_process(
        COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/test.sh
)

# Запуск тестов после сборки qap
add_custom_command(TARGET qap POST_BUILD
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
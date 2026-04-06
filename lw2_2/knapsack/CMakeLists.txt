cmake_minimum_required(VERSION 4.0)
project(knapsack)

set(CMAKE_CXX_STANDARD 20)

add_executable(knapsack main.cpp)

# копирование тестового окружения
configure_file(test.sh ${CMAKE_CURRENT_BINARY_DIR}/test.sh COPYONLY)
configure_file(input.txt ${CMAKE_CURRENT_BINARY_DIR}/input.txt COPYONLY)

# Установка прав на выполнение для .sh
execute_process(
        COMMAND chmod +x ${CMAKE_CURRENT_BINARY_DIR}/test.sh
)

# Запуск тестов после сборки knapsack
add_custom_command(TARGET knapsack POST_BUILD
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
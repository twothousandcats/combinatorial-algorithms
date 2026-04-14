cmake_minimum_required(VERSION 3.20)
project(task2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# warnings
#if (MSVC)
#    add_compile_options(/W4 /WX-)
#else ()
#    add_compile_options(-Wall -Wextra -pedantic -Werror=return-type)
#endif ()

# logic
add_library(task2_logic STATIC
        src/BoruvkaMST.cpp
        src/DisjointSet.cpp
        src/Geometry.cpp
        src/Graph.cpp
        src/InputReader.cpp
        src/Point.cpp
        src/SteinerTree.cpp
        src/Visualizer.cpp
)
target_include_directories(
        task2_logic
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(task2 src/main.cpp)
target_link_libraries(task2
        PRIVATE
        task2_logic
)

# Tests
find_package(Catch2 REQUIRED)

add_executable(task2-tests tests/tests.cpp)
target_link_libraries(task2-tests
        PRIVATE
        task2_logic
        Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME UnitTests COMMAND task2-tests)
set_property(TEST UnitTests PROPERTY LABELS "Unit")

# Auto-run tests after build
add_custom_command(TARGET task2-tests POST_BUILD
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R UnitTests
        COMMENT "Running unit tests..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
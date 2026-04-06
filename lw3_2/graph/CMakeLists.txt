cmake_minimum_required(VERSION 4.0)
project(graph)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# warnings
if (MSVC)
    add_compile_options(/W4 /WX-)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror=return-type)
endif ()

# graph
add_library(graph_logic STATIC src/Graph.cpp)
target_include_directories(graph_logic
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(graph src/main.cpp)
target_link_libraries(graph
        PRIVATE
        graph_logic
)

# Tests
find_package(Catch2 REQUIRED)

add_executable(graph-tests tests/tests.cpp)
target_link_libraries(graph-tests
        PRIVATE
        graph_logic
        Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME UnitTests COMMAND graph-tests)
set_property(TEST UnitTests PROPERTY LABELS "Unit")

# Auto-run tests after build
add_custom_command(TARGET graph-tests POST_BUILD
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R UnitTests
        COMMENT "Running unit tests..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
cmake_minimum_required(VERSION 3.20)
project(genetic)

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
add_library(genetic_logic STATIC
        src/CBruteForceSolver.cpp
        src/CFileLoader.cpp
        src/CGeneticSolver.cpp
        src/utils.cpp
)
target_include_directories(
        genetic_logic
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(genetic src/main.cpp)
target_link_libraries(genetic
        PRIVATE
        genetic_logic
)

# Tests
find_package(Catch2 REQUIRED)

add_executable(genetic-tests tests/tests.cpp)
target_link_libraries(genetic-tests
        PRIVATE
        genetic_logic
        Catch2::Catch2WithMain
)

include(CTest)
add_test(NAME UnitTests COMMAND genetic-tests)
set_property(TEST UnitTests PROPERTY LABELS "Unit")

# Auto-run tests after build
add_custom_command(TARGET genetic-tests POST_BUILD
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R UnitTests
        COMMENT "Running unit tests..."
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
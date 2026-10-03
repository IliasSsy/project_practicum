cmake_minimum_required(VERSION 4.3)
project(project_practicum)

set(CMAKE_CXX_STANDARD 14)

add_executable(project_practicum
        main.cpp
        hello.cpp
        hello_err.cpp
        abc.cpp
        linear_equation.cpp
        square_equation.cpp
        array_stats.cpp
        sum_until_zero.cpp)

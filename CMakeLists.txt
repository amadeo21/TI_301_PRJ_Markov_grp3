cmake_minimum_required(VERSION 3.16)
project(TI301_Markov_Project C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(TI301_Markov
        src/main_markov.c
        src/graph.c
        src/tarjan.c
        src/hasse.c
        src/matrix.c
        src/utils.c
)

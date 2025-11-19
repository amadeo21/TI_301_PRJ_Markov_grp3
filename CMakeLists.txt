cmake_minimum_required(VERSION 3.10)
project(TI_301_PRJ_Markov C)

set(CMAKE_C_STANDARD 99)

add_executable(markov_project
        main_markov.c
        markov.c
        hasse.c
        utils.c
)


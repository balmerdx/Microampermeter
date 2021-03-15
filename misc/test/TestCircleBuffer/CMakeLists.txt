cmake_minimum_required(VERSION 3.5)

project(TestCircleBuffer LANGUAGES C)

add_executable(TestCircleBuffer main.c ../../../h7code/Core/Src/measure/circle_buf.c)

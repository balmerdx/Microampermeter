cmake_minimum_required(VERSION 3.5)

project(TestCircleBuffer LANGUAGES C)

#добавить директорию, в которой будет CMakeLists.txt
#add_subdirectory(../../../h7code/Core/Src/histogram out_hisogramm)

#добавить все файлы из директории
#file (GLOB CPP_FILES ../../../h7code/Core/Src/histogram/*.c )
#file (GLOB H_FILES ../../../h7code/Core/Src/histogram/*.h)

set(CIRCLE_DIR "../../../h7code/Core/Src/measure")

add_executable(TestCircleBuffer main.c ${CIRCLE_DIR}/circle_buf.c ${CIRCLE_DIR}/circle_buf.h
    ${CPP_FILES} ${H_FILES}
    )

target_include_directories(TestCircleBuffer PRIVATE ${CIRCLE_DIR})

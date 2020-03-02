cmake_minimum_required(VERSION 3.4.1)

project(threadwin32)

add_compile_options(-DDEBUG )

set(PTHREAD_WIN32_ROOT_DIR ${PROJECT_SOURCE_DIR}/)
set(PTHREAD_WIN32_INC_DIR ${PTHREAD_WIN32_ROOT_DIR}/inc)
set(PTHREAD_WIN32_SRC_DIR ${PTHREAD_WIN32_ROOT_DIR}/src)
file(GLOB PTHREAD_WIN32_SRCS ${PTHREAD_WIN32_SRC_DIR}/*.c)

include_directories(pthreadVC ${PTHREAD_WIN32_INC_DIR})

link_directories(pthreadVC ${PTHREAD_WIN32_SRC_DIR}/)

add_library(pthreadVC ${PTHREAD_WIN32_SRCS})
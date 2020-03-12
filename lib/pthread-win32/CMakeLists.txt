cmake_minimum_required(VERSION 3.4.1)

project(threadwin32)

add_compile_options(-DDEBUG )

set(PTHREAD_WIN32_ROOT_DIR ${PROJECT_SOURCE_DIR}/)
set(PTHREAD_WIN32_INC_DIR ${PTHREAD_WIN32_ROOT_DIR}/inc)
set(PTHREAD_WIN32_SRC_DIR ${PTHREAD_WIN32_ROOT_DIR}/src)
#file(GLOB PTHREAD_WIN32_SRCS ${PTHREAD_WIN32_SRC_DIR}/*.c)
file(GLOB PTHREAD_WIN32_SRCS ${PTHREAD_WIN32_SRC_DIR}/attr.c 
 ${PTHREAD_WIN32_SRC_DIR}/barrier.c
 ${PTHREAD_WIN32_SRC_DIR}/cancel.c 
 ${PTHREAD_WIN32_SRC_DIR}/cleanup.c 
 ${PTHREAD_WIN32_SRC_DIR}/condvar.c 
 ${PTHREAD_WIN32_SRC_DIR}/create.c 
 ${PTHREAD_WIN32_SRC_DIR}/dll.c 
 ${PTHREAD_WIN32_SRC_DIR}/exit.c 
 ${PTHREAD_WIN32_SRC_DIR}/fork.c 
 ${PTHREAD_WIN32_SRC_DIR}/global.c 
 ${PTHREAD_WIN32_SRC_DIR}/misc.c 
 ${PTHREAD_WIN32_SRC_DIR}/mutex.c 
 ${PTHREAD_WIN32_SRC_DIR}/private.c 
 ${PTHREAD_WIN32_SRC_DIR}/rwlock.c 
 ${PTHREAD_WIN32_SRC_DIR}/sched.c 
 ${PTHREAD_WIN32_SRC_DIR}/semaphore.c 
 ${PTHREAD_WIN32_SRC_DIR}/spin.c 
 ${PTHREAD_WIN32_SRC_DIR}/sync.c 
 ${PTHREAD_WIN32_SRC_DIR}/tsd.c 
 ${PTHREAD_WIN32_SRC_DIR}/nonportable.c)
    
include_directories(pthreadVC2 ${PTHREAD_WIN32_INC_DIR})

link_directories(pthreadVC2 ${PTHREAD_WIN32_SRC_DIR}/)

add_library(pthreadVC2 SHARED ${PTHREAD_WIN32_SRCS})

target_link_libraries(pthreadVC2 ws2_32 iphlpapi kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
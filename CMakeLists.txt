cmake_minimum_required(VERSION 3.4.1)

project(pupnp)

add_subdirectory(upnp)
add_subdirectory(ixml)

add_compile_options(-DDEBUG 
-DWIN32
-D_USRDLL
-DLIBUPNP_EXPORTS
-DUPNP_USE_MSVCPP
-D_CRT_NONSTDC_NO_WARNINGS
-D_CRT_NONSTDC_NO_DEPRECATE
-D_CRT_SECURE_NO_WARNINGS
-D_CRT_SECURE_NO_DEPRECATE
-D_SECURE_SCL
-D_SCL_SECURE_NO_WARNINGS
-D_SCL_SECURE_NO_DEPRECATE
-D_AFX_SECURE_NO_WARNINGS
-D_AFX_SECURE_NO_DEPRECATE
-D_SECURE_ATL
-D_ATL_NO_COM_SUPPORT
-D_ATL_SECURE_NO_WARNINGS
-D_ATL_SECURE_NO_DEPRECATE
)

set(PTHREAD_WIN32_ROOT_DIR ${PROJECT_SOURCE_DIR}/lib/pthread-win32)
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

set(IXML_ROOT_DIR ${PROJECT_SOURCE_DIR}/ixml)
set(IXML_INC_DIR ${IXML_ROOT_DIR}/inc)
set(IXML_SRC_DIR ${IXML_ROOT_DIR}/src)
file(GLOB IXML_SRCS ${IXML_SRC_DIR}/*.c)

set(UPNP_ROOT_DIR ${PROJECT_SOURCE_DIR}/upnp)
set(UPNP_INC_DIR ${UPNP_ROOT_DIR}/inc)
set(UPNP_SRC_DIR ${UPNP_ROOT_DIR}/src)
file(GLOB UPNP_SRCS ${UPNP_SRC_DIR}/*.c)

set(UPNP_API_ROOT_DIR ${UPNP_SRC_DIR}/api)
set(UPNP_API_INC_DIR ${UPNP_API_ROOT_DIR})
set(UPNP_API_SRC_DIR ${UPNP_API_ROOT_DIR})
file(GLOB UPNP_API_SRCS ${UPNP_API_SRC_DIR}/*.c)

set(UPNP_GENA_ROOT_DIR ${UPNP_SRC_DIR}/gena)
set(UPNP_GENA_INC_DIR ${UPNP_GENA_ROOT_DIR})
set(UPNP_GENA_SRC_DIR ${UPNP_GENA_ROOT_DIR})
file(GLOB UPNP_GENA_SRCS ${UPNP_GENA_SRC_DIR}/*.c)

set(UPNP_GENLIB_ROOT_DIR ${UPNP_SRC_DIR}/genlib)
set(UPNP_GENLIB_INC_DIR ${UPNP_GENLIB_ROOT_DIR})
set(UPNP_GENLIB_SRC_DIR ${UPNP_GENLIB_ROOT_DIR})
file(GLOB UPNP_GENLIB_SRCS ${UPNP_GENLIB_SRC_DIR}/*.c)

set(UPNP_CLI_TAB_ROOT_DIR ${UPNP_GENLIB_ROOT_DIR}/client_table)
set(UPNP_CLI_TAB_INC_DIR ${UPNP_CLI_TAB_ROOT_DIR})
set(UPNP_CLI_TAB_SRC_DIR ${UPNP_CLI_TAB_ROOT_DIR})
file(GLOB UPNP_CLI_TAB_SRCS ${UPNP_CLI_TAB_SRC_DIR}/*.c)

set(UPNP_SVR_ROOT_DIR ${UPNP_GENLIB_ROOT_DIR}/miniserver)
set(UPNP_SVR_INC_DIR ${UPNP_SVR_ROOT_DIR})
set(UPNP_SVR_SRC_DIR ${UPNP_SVR_ROOT_DIR})
file(GLOB UPNP_SVR_SRCS ${UPNP_SVR_SRC_DIR}/*.c)

set(UPNP_NET_ROOT_DIR ${UPNP_GENLIB_ROOT_DIR}/net)
set(UPNP_NET_INC_DIR ${UPNP_NET_ROOT_DIR})
set(UPNP_NET_SRC_DIR ${UPNP_NET_ROOT_DIR})
file(GLOB UPNP_NET_SRCS ${UPNP_NET_SRC_DIR}/*.c ${UPNP_NET_SRC_DIR}/http/*.c ${UPNP_NET_SRC_DIR}/uri/*.c)

set(UPNP_SVR_TAB_ROOT_DIR ${UPNP_GENLIB_ROOT_DIR}/service_table)
set(UPNP_SVR_TAB_INC_DIR ${UPNP_SVR_TAB_ROOT_DIR})
set(UPNP_SVR_TAB_SRC_DIR ${UPNP_SVR_TAB_ROOT_DIR})
file(GLOB UPNP_SVR_TAB_SRCS ${UPNP_SVR_TAB_SRC_DIR}/*.c)

set(UPNP_UTIL_ROOT_DIR ${UPNP_GENLIB_ROOT_DIR}/util)
set(UPNP_UTIL_INC_DIR ${UPNP_UTIL_ROOT_DIR})
set(UPNP_UTIL_SRC_DIR ${UPNP_UTIL_ROOT_DIR})
file(GLOB UPNP_UTIL_SRCS ${UPNP_UTIL_SRC_DIR}/*.c)
file(GLOB UPNP_GENLIB_ALL_SRCS ${UPNP_UTIL_SRCS} ${UPNP_SVR_TAB_SRCS} ${UPNP_NET_SRCS} ${UPNP_SVR_SRCS} ${UPNP_CLI_TAB_SRCS} ${UPNP_GENLIB_SRCS})

set(UPNP_SOAP_ROOT_DIR ${UPNP_SRC_DIR}/soap)
set(UPNP_SOAP_INC_DIR ${UPNP_SOAP_ROOT_DIR})
set(UPNP_SOAP_SRC_DIR ${UPNP_SOAP_ROOT_DIR})
file(GLOB UPNP_SOAP_SRCS ${UPNP_SOAP_SRC_DIR}/*.c)

set(UPNP_SSDP_ROOT_DIR ${UPNP_SRC_DIR}/ssdp)
set(UPNP_SSDP_INC_DIR ${UPNP_SSDP_ROOT_DIR})
set(UPNP_SSDP_SRC_DIR ${UPNP_SSDP_ROOT_DIR})
file(GLOB UPNP_SSDP_SRCS ${UPNP_SSDP_SRC_DIR}/*.c)

set(UPNP_THREAD_UTIL_ROOT_DIR ${UPNP_SRC_DIR}/threadutil)
set(UPNP_THREAD_UTIL_INC_DIR ${UPNP_THREAD_UTIL_ROOT_DIR})
set(UPNP_THREAD_UTIL_SRC_DIR ${UPNP_THREAD_UTIL_ROOT_DIR})
file(GLOB UPNP_THREAD_UTIL_SRCS ${UPNP_THREAD_UTIL_SRC_DIR}/*.c)

set(UPNP_URL_CONFIG_ROOT_DIR ${UPNP_SRC_DIR}/urlconfig)
set(UPNP_URL_CONFIG_INC_DIR ${UPNP_URL_CONFIG_ROOT_DIR})
set(UPNP_URL_CONFIG_SRC_DIR ${UPNP_URL_CONFIG_ROOT_DIR})
file(GLOB UPNP_URL_CONFIG_SRCS ${UPNP_URL_CONFIG_SRC_DIR}/*.c)

set(UPNP_UUID_ROOT_DIR ${UPNP_SRC_DIR}/uuid)
set(UPNP_UUID_INC_DIR ${UPNP_UUID_ROOT_DIR})
set(UPNP_UUID_SRC_DIR ${UPNP_UUID_ROOT_DIR})
file(GLOB UPNP_UUID_SRCS ${UPNP_UUID_SRC_DIR}/*.c)
file(GLOB UPNP_ALL_SRCS ${UPNP_SRCS} ${UPNP_API_SRCS} ${UPNP_GENA_SRCS}
 ${UPNP_GENLIB_ALL_SRCS} ${UPNP_SOAP_SRCS} ${UPNP_SSDP_SRCS} ${UPNP_THREAD_UTIL_SRCS} ${UPNP_URL_CONFIG_SRCS} ${UPNP_UUID_SRCS})
list(REMOVE_ITEM UPNP_ALL_SRCS ${UPNP_SRC_DIR}/inet_pton.c)

set(PTHREAD_WIN32_ROOT_DIR ${PROJECT_SOURCE_DIR}/lib/pthread-win32)
set(PTHREAD_WIN32_INC_DIR ${PTHREAD_WIN32_ROOT_DIR}/inc)
set(PTHREAD_WIN32_LIB_DIR ${PTHREAD_WIN32_ROOT_DIR}/lib)

set(OPENSSL_ROOT_DIR ${PROJECT_SOURCE_DIR}/lib/openssl)
set(OPENSSL_INC_DIR ${OPENSSL_ROOT_DIR}/include)
set(OPENSSL_LIB_DIR ${OPENSSL_ROOT_DIR}/lib)

set(UPNP_SAMPLE_ROOT_DIR ${UPNP_ROOT_DIR}/sample)
set(UPNP_SAMPLE_COMM_DIR ${UPNP_SAMPLE_ROOT_DIR}/common)
set(UPNP_SAMPLE_MAIN_DIR ${UPNP_SAMPLE_ROOT_DIR}/linux)
file(GLOB UPNP_CTRLPT_MAIN_SRCS ${UPNP_SAMPLE_COMM_DIR}/*.c ${UPNP_SAMPLE_MAIN_DIR}/tv_ctrlpt_main.c)
file(GLOB UPNP_DEVICE_MAIN_SRCS ${UPNP_SAMPLE_COMM_DIR}/*.c ${UPNP_SAMPLE_MAIN_DIR}/tv_device_main.c)
file(GLOB UPNP_COMBO_MAIN_SRCS ${UPNP_SAMPLE_COMM_DIR}/*.c ${UPNP_SAMPLE_MAIN_DIR}/tv_combo_main.c)
message(${PROJECT_SOURCE_DIR})

include_directories(ctrlpt ${PTHREAD_WIN32_INC_DIR} ${IXML_INC_DIR} ${UPNP_INC_DIR} ${UPNP_API_INC_DIR} ${UPNP_GENA_INC_DIR}
 ${UPNP_GENLIB_INC_DIR} ${UPNP_CLI_TAB_INC_DIR} ${UPNP_SVR_INC_DIR} ${UPNP_NET_INC_DIR} ${UPNP_SVR_TAB_INC_DIR} ${UPNP_UTIL_INC_DIR}
 ${UPNP_SOAP_INC_DIR} ${UPNP_SSDP_INC_DIR} ${UPNP_THREAD_UTIL_INC_DIR} ${UPNP_URL_CONFIG_INC_DIR} ${UPNP_UUID_INC_DIR}
 ${UPNP_SRC_DIR}/inc ${IXML_SRC_DIR}/inc ${UPNP_SAMPLE_COMM_DIR} ${PTHREAD_WIN32_SRC_DIR} ${OPENSSL_INC_DIR})
link_directories(ctrlpt ${PROJECT_SOURCE_DIR}/upnp/lib ${PTHREAD_WIN32_LIB_DIR} ${UPNP_SAMPLE_COMM_DIR} ${OPENSSL_LIB_DIR})
add_executable(ctrlpt ${IXML_SRCS} ${UPNP_ALL_SRCS} ${UPNP_CTRLPT_MAIN_SRCS})
target_link_libraries(ctrlpt libcrypto libssl pthreadVC2 ws2_32 iphlpapi kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)

include_directories(device ${PTHREAD_WIN32_INC_DIR} ${IXML_INC_DIR} ${UPNP_INC_DIR} ${UPNP_API_INC_DIR} ${UPNP_GENA_INC_DIR}
 ${UPNP_GENLIB_INC_DIR} ${UPNP_CLI_TAB_INC_DIR} ${UPNP_SVR_INC_DIR} ${UPNP_NET_INC_DIR} ${UPNP_SVR_TAB_INC_DIR} ${UPNP_UTIL_INC_DIR}
 ${UPNP_SOAP_INC_DIR} ${UPNP_SSDP_INC_DIR} ${UPNP_THREAD_UTIL_INC_DIR} ${UPNP_URL_CONFIG_INC_DIR} ${UPNP_UUID_INC_DIR}
 ${UPNP_SRC_DIR}/inc ${IXML_SRC_DIR}/inc ${UPNP_SAMPLE_COMM_DIR} ${PTHREAD_WIN32_SRC_DIR} ${OPENSSL_INC_DIR})
link_directories(device ${PROJECT_SOURCE_DIR}/upnp/lib ${PTHREAD_WIN32_LIB_DIR} ${UPNP_SAMPLE_COMM_DIR} ${OPENSSL_LIB_DIR})
add_executable(device ${IXML_SRCS} ${UPNP_ALL_SRCS} ${UPNP_DEVICE_MAIN_SRCS})
target_link_libraries(device libcrypto libssl pthreadVC2 ws2_32 iphlpapi kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
set(CMAKE_DEVICE_FLAGS "${CMAKE_CXX_FLAGS} -DUPNP_API")
set_target_properties(device PROPERTIES COMPILE_FLAGS ${CMAKE_DEVICE_FLAGS})

include_directories(combo ${PTHREAD_WIN32_INC_DIR} ${IXML_INC_DIR} ${UPNP_INC_DIR} ${UPNP_API_INC_DIR} ${UPNP_GENA_INC_DIR}
 ${UPNP_GENLIB_INC_DIR} ${UPNP_CLI_TAB_INC_DIR} ${UPNP_SVR_INC_DIR} ${UPNP_NET_INC_DIR} ${UPNP_SVR_TAB_INC_DIR} ${UPNP_UTIL_INC_DIR}
 ${UPNP_SOAP_INC_DIR} ${UPNP_SSDP_INC_DIR} ${UPNP_THREAD_UTIL_INC_DIR} ${UPNP_URL_CONFIG_INC_DIR} ${UPNP_UUID_INC_DIR}
 ${UPNP_SRC_DIR}/inc ${IXML_SRC_DIR}/inc ${UPNP_SAMPLE_COMM_DIR} ${PTHREAD_WIN32_SRC_DIR} ${OPENSSL_INC_DIR})
link_directories(combo ${PROJECT_SOURCE_DIR}/upnp/lib ${PTHREAD_WIN32_LIB_DIR} ${UPNP_SAMPLE_COMM_DIR} ${OPENSSL_LIB_DIR})
add_executable(combo ${IXML_SRCS} ${UPNP_ALL_SRCS} ${UPNP_COMBO_MAIN_SRCS})
target_link_libraries(combo libcrypto libssl pthreadVC2 ws2_32 iphlpapi kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
set(CMAKE_COMBO_FLAGS "${CMAKE_CXX_FLAGS} -DUPNP_API")
set_target_properties(combo PROPERTIES COMPILE_FLAGS ${CMAKE_COMBO_FLAGS})
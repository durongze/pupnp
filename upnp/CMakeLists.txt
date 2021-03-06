cmake_minimum_required(VERSION 3.4.1)

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
file(GLOB UPNP_GENLIB_ALL_SRCS ${UPNP_UTIL_SRCS} ${UPNP_SVR_TAB_SRCS} ${UPNP_NET_SRCS} ${UPNP_SVR_SRCS} ${UPNP_CLI_TAB_SRCS} ${UPNP_GENLIB_SRCS} )

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

message(${PROJECT_SOURCE_DIR})

include_directories(upnp ${PTHREAD_WIN32_INC_DIR} ${IXML_INC_DIR} ${UPNP_INC_DIR} ${UPNP_API_INC_DIR} ${UPNP_GENA_INC_DIR}
 ${UPNP_GENLIB_INC_DIR} ${UPNP_CLI_TAB_INC_DIR} ${UPNP_SVR_INC_DIR} ${UPNP_NET_INC_DIR} ${UPNP_SVR_TAB_INC_DIR} ${UPNP_UTIL_INC_DIR}
 ${UPNP_SOAP_INC_DIR} ${UPNP_SSDP_INC_DIR} ${UPNP_THREAD_UTIL_INC_DIR} ${UPNP_URL_CONFIG_INC_DIR} ${UPNP_UUID_INC_DIR}
 ${UPNP_SRC_DIR}/inc ${IXML_SRC_DIR}/inc ${OPENSSL_INC_DIR})

link_directories(upnp ${PROJECT_SOURCE_DIR}/upnp/lib ${PTHREAD_WIN32_LIB_DIR} ${OPENSSL_LIB_DIR})

add_library(upnp SHARED  ${IXML_SRCS} ${UPNP_ALL_SRCS})

target_link_libraries(upnp pthreadVC2 ws2_32 iphlpapi kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
 















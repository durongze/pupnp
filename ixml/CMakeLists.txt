cmake_minimum_required(VERSION 3.4.1)

add_compile_options(-DDEBUG
-DWIN32
-D_USRDLL
-DLIBUPNP_EXPORTS
-DUPNP_USE_MSVCPP
-DIXML_HAVE_SCRIPTSUPPORT
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
-D_ATL_SECURE_NO_DEPRECATE)

set(UPNP_ROOT_DIR ${PROJECT_SOURCE_DIR}/upnp)
set(UPNP_INC_DIR ${UPNP_ROOT_DIR}/inc)
set(UPNP_SRC_DIR ${UPNP_ROOT_DIR}/src)

set(IXML_ROOT_DIR ${PROJECT_SOURCE_DIR}/ixml)
set(IXML_INC_DIR ${IXML_ROOT_DIR}/inc)
set(IXML_SRC_DIR ${IXML_ROOT_DIR}/src)
file(GLOB IXML_SRCS ${IXML_SRC_DIR}/*.c)

include_directories(ixml ${IXML_INC_DIR} ${IXML_SRC_DIR}/inc ${UPNP_INC_DIR} ${UPNP_SRC_DIR}/inc)
link_directories(ixml ${IXML_SRC_DIR})
add_library(ixml SHARED ${IXML_SRCS} )
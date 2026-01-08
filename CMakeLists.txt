cmake_minimum_required(VERSION 3.8)
project(easy_deployment)

set(ENABLE_ORT ON CACHE BOOL "Enable inference feature (set to ON)" FORCE)
# set(ENABLE_TENSORRT ON CACHE BOOL "Enable inference feature (set to ON)" FORCE)

# set lib type and lib output path
MESSAGE("\n******easy_deployment CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET (DEPLOY_LIB_TYPE "SHARED")
    SET (DEPLOY_LIB_PATH ${PROJECT_SOURCE_DIR}/lib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
    SET (DEPLOY_LIB_TYPE "STATIC")
    SET (DEPLOY_LIB_PATH ${PROJECT_SOURCE_DIR}/lib_android)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    SET (DEPLOY_LIB_TYPE "SHARED")
    SET (DEPLOY_LIB_PATH ${PROJECT_SOURCE_DIR}/lib_win)
endif ()
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DEPLOY_LIB_PATH}/lib)   # .lib (静态库 / import lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DEPLOY_LIB_PATH}/bin)   # .dll
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DEPLOY_LIB_PATH}/bin)   # .exe

add_subdirectory(deploy_core)
add_subdirectory(deploy_utils)
add_subdirectory(inference_core)

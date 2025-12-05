cmake_minimum_required(VERSION 3.8)
project(easy_deployment)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(ENABLE_ORT ON CACHE BOOL "Enable inference feature (set to ON)" FORCE)
# set(ENABLE_TENSORRT ON CACHE BOOL "Enable inference feature (set to ON)" FORCE)

add_subdirectory(deploy_core)
add_subdirectory(deploy_utils)
add_subdirectory(inference_core)

cmake_minimum_required(VERSION 3.8)
project(trt_core)


add_compile_options(-std=c++17)
add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)


find_package(CUDA REQUIRED)


set(source_file src/trt_core.cpp
                src/trt_core_factory.cpp)


include_directories(
  include
  ${CUDA_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} SHARED ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
  deploy_core
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ${CUDA_LIBRARIES}
  nvinfer
  nvonnxparser
  nvinfer_plugin
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${PACKAGE_NAME}>"
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${ONNXRUNTIME_INCLUDE_DIRS}"
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PACKAGE_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/
  DESTINATION include/${PACKAGE_NAME}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_ORT)

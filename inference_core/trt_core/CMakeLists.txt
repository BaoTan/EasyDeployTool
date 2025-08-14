add_compile_options(-std=c++17)
add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
set(CMAKE_CXX_STANDARD 17)

set(MODULE_NAME trt_core)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(CUDA REQUIRED)

set(source_file src/trt_core.cpp
                src/trt_core_factory.cpp)
add_library(${MODULE_NAME} SHARED ${source_file})
target_link_libraries(${MODULE_NAME} PUBLIC
  deploy_core
)
target_link_libraries(${MODULE_NAME} PRIVATE
  ${CUDA_LIBRARIES}
  nvinfer
  nvonnxparser
  nvinfer_plugin
)
target_include_directories(${MODULE_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
)
target_include_directories(${MODULE_NAME}
  PRIVATE
    "${CUDA_INCLUDE_DIRS}"
)

install(TARGETS ${MODULE_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

target_compile_definitions(${MODULE_NAME} PUBLIC ENABLE_ORT)

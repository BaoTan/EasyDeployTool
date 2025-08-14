cmake_minimum_required(VERSION 3.8)
project(image_processing_utils)

add_compile_options(-std=c++17)
add_compile_options(-O3)
set(CMAKE_CXX_STANDARD 17)

if(ENABLE_TENSORRT)
  find_package(CUDA REQUIRED)
  set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
  enable_language(CUDA)
  include_directories(${CUDA_INCLUDE_DIRS})
endif()

find_package(OpenCV REQUIRED)

set(source_file
    src/image_processing_cpu.cpp
)

if(ENABLE_TENSORRT)
  find_package(CUDA REQUIRED)
  set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)
  enable_language(CUDA)
  include_directories(${CUDA_INCLUDE_DIRS})
  list(APPEND source_file
          src/image_processing_cuda_kernel.cu
          src/image_processing_cuda.cpp
  )
endif()

add_library(${PROJECT_NAME} SHARED ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OpenCV_LIBS}
  deploy_core
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${PACKAGE_NAME}>"
    "${OpenCV_INCLUDE_DIRS}"
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PACKAGE_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/
  DESTINATION include/${PACKAGE_NAME}
)

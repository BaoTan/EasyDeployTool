cmake_minimum_required(VERSION 3.8)
project(image_processing_utils)

add_compile_options(-std=c++17)
add_compile_options(-O3)
set(CMAKE_CXX_STANDARD 17)

# find libs
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

# create lib
include_directories(
  include
  ${OpenCV_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} ${DEPLOY_LIB_TYPE} ${source_file})

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OpenCV_LIBS}
  deploy_core
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WINDOWS_EXPORT_ALL_SYMBOLS ON
	)
endif()

# install
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
cmake_minimum_required(VERSION 3.8)
project(ort_core)

add_compile_options(-std=c++17)
if(MSVC)
  add_compile_options(-O3 -fPIC)
else()
  add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
endif()
set(CMAKE_CXX_STANDARD 17)

# find libs
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
  ## linux x86_64 onnxruntime lib
  MESSAGE(STATUS "Building OnnxRuntime-Core on x86_64 platform ...")
  if (CMAKE_SYSTEM_NAME MATCHES "Linux")
      SET(ONNX_ROOT_PATH /home/paco/work_envs/dl_env/onnxruntime-linux-x64-gpu-1.16.1)
      SET(ONNXRUNTIME_INCLUDE_DIRS ${ONNX_ROOT_PATH}/include)
      FILE(GLOB ONNXRUNTIME_LIBS ${ONNX_ROOT_PATH}/lib/*.so)
  elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
      SET(ONNX_ROOT_PATH D:/05_Paco/software/3rd/onnxruntime-win-x64-gpu-1.16.1)
      SET(ONNXRUNTIME_INCLUDE_DIRS ${ONNX_ROOT_PATH}/include)
      link_directories(${ONNX_ROOT_PATH}/lib)
      FILE(GLOB ONNXRUNTIME_LIBS ${ONNX_ROOT_PATH}/lib/*.lib)
  endif ()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  message(STATUS "Building OnnxRuntime-Core on aarch64 platform ...")
  set(ONNXRUNTIME_INCLUDE_DIRS onnxruntime-pkg-aarch64/include)
  FILE(GLOB ONNXRUNTIME_LIBS onnxruntime-pkg-aarch64/lib/*.so)
else()
  message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}. \
                      Supported architectures are x86_64 and aarch64.")
endif()

message("\n******ONNXRUNTIME_INCLUDE_DIRS = ${ONNXRUNTIME_INCLUDE_DIRS}")

# create lib
set(source_file
  src/ort_core.cpp
  src/ort_core_factory.cpp
)

include_directories(
  include
  ${ONNXRUNTIME_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} ${DEPLOY_LIB_TYPE} ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${ONNXRUNTIME_LIBS}
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



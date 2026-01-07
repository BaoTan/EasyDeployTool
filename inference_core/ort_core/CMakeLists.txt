cmake_minimum_required(VERSION 3.8)
project(ort_core)

add_compile_options(-std=c++17)
if(MSVC)
	add_compile_options(-O3 -Wdeprecated -fPIC)
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

# create mingw onnxruntime lib
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  ## 主 ONNX Runtime
  add_library(onnxruntime ${DEPLOY_LIB_TYPE} IMPORTED)
  set_target_properties(onnxruntime PROPERTIES
    IMPORTED_LOCATION "${ONNX_ROOT_PATH}/lib/onnxruntime.dll"
    IMPORTED_IMPLIB   "${ONNX_ROOT_PATH}/lib/libonnxruntime.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${ONNXRUNTIME_INCLUDE_DIRS}"
  )
  target_include_directories(onnxruntime INTERFACE
      "${ONNXRUNTIME_INCLUDE_DIRS}"
  )
endif()

# create lib
set(LIBRARY_OUTPUT_PATH ${DEPLOY_LIB_PATH})
set(source_file
  src/ort_core.cpp
  src/ort_core_factory.cpp
)

include_directories(
  include
  ${ONNXRUNTIME_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} ${DEPLOY_LIB_TYPE} ${source_file})

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_link_libraries(${PROJECT_NAME} PUBLIC
    ${ONNXRUNTIME_LIBS}
    deploy_core
  )
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_link_libraries(${PROJECT_NAME} PUBLIC
    onnxruntime
    deploy_core
  )
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# install
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib)



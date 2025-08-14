add_compile_options(-std=c++17)
add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
set(CMAKE_CXX_STANDARD 17)

set(MODULE_NAME ort_core)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  message(STATUS "Building OnnxRuntime-Core on x86_64 platform ...")
  set(ONNXRUNTIME_INCLUDE_DIRS onnxruntime-pkg-x86/include)
  FILE(GLOB ONNXRUNTIME_LIBS onnxruntime-pkg-x86/lib/*.so)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  message(STATUS "Building OnnxRuntime-Core on aarch64 platform ...")
  set(ONNXRUNTIME_INCLUDE_DIRS onnxruntime-pkg-aarch64/include)
  FILE(GLOB ONNXRUNTIME_LIBS onnxruntime-pkg-aarch64/lib/*.so)
else()
  message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}. \
                      Supported architectures are x86_64 and aarch64.")
endif()


set(source_file
  src/ort_core.cpp
  src/ort_core_factory.cpp
)
add_library(${MODULE_NAME} SHARED ${source_file})
target_link_libraries(${MODULE_NAME} PUBLIC
  deploy_core
)
target_link_libraries(${MODULE_NAME} PRIVATE
  ${ONNXRUNTIME_LIBS}
)
target_include_directories(${MODULE_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
)
target_include_directories(${MODULE_NAME}
  PRIVATE
    "${ONNXRUNTIME_INCLUDE_DIRS}"
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

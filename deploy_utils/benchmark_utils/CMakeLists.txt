cmake_minimum_required(VERSION 3.8)
project(benchmark_utils)

add_compile_options(-std=c++17)
add_compile_options(-O3)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(glog REQUIRED)
find_package(benchmark REQUIRED)

set(source_file
    src/detection_2d_benchmark_utils.cpp
    src/sam_benchmark_utils.cpp
    src/stereo_matching_benchmark_utils.cpp
)
add_library(${PROJECT_NAME} SHARED ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OpenCV_LIBS}
  deploy_core
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  benchmark::benchmark
  glog::glog
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

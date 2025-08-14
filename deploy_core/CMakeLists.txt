cmake_minimum_required(VERSION 3.8)
project(deploy_core)

add_compile_options(-std=c++17)
add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

set(source_file src/base_infer_core.cpp
                src/base_detection.cpp
                src/base_sam.cpp
                src/base_stereo.cpp
                src/base_mono_stereo.cpp
)
add_library(${PROJECT_NAME} SHARED ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OpenCV_LIBS}
  common_utils
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

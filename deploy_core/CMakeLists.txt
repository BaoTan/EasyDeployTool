add_compile_options(-std=c++17)
add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
set(CMAKE_CXX_STANDARD 17)

set(MODULE_NAME deploy_core)

find_package(OpenCV REQUIRED)

set(source_file src/base_infer_core.cpp
                src/base_detection.cpp
                src/base_sam.cpp
                src/base_stereo.cpp
                src/base_mono_stereo.cpp
)
add_library(${MODULE_NAME} SHARED ${source_file})
target_link_libraries(${MODULE_NAME} PUBLIC
  ${OpenCV_LIBS}
  common_utils
)
target_include_directories(${MODULE_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
    "${OpenCV_INCLUDE_DIRS}"
)

install(TARGETS ${MODULE_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

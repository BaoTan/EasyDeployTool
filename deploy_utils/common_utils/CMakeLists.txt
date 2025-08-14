cmake_minimum_required(VERSION 3.8)
project(common_utils)

add_compile_options(-std=c++17)
add_compile_options(-O3)
set(CMAKE_CXX_STANDARD 17)

set(source_file
  src/log.cpp
)
add_library(${PROJECT_NAME} SHARED ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${PACKAGE_NAME}>"
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PACKAGE_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/
  DESTINATION include/${PACKAGE_NAME}
)

if (ENABLE_DEBUG_OUTPUT)
  target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_DEBUG_OUTPUT)
endif()

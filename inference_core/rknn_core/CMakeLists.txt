add_compile_options(-std=c++17)
add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
set(CMAKE_CXX_STANDARD 17)

set(MODULE_NAME rknn_core)

set(source_file src/rknn_core.cpp
                src/rknn_core_factory.cpp)

add_library(${MODULE_NAME} SHARED ${source_file})
target_link_libraries(${MODULE_NAME} PUBLIC
  deploy_core
)
target_link_libraries(${MODULE_NAME} PRIVATE
  rknnrt
)
target_include_directories(${MODULE_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
    "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
)

install(TARGETS ${MODULE_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

target_compile_definitions(${MODULE_NAME} PUBLIC ENABLE_RKNN)

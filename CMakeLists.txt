cmake_minimum_required(VERSION 3.8)
project(easy_deploy_tool VERSION 1.2.3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PACKAGE_NAME ${PROJECT_NAME})

add_subdirectory(deploy_core)
add_subdirectory(deploy_utils)
add_subdirectory(inference_core)


install(EXPORT ${PACKAGE_NAME}Targets
    FILE ${PACKAGE_NAME}Targets.cmake
    NAMESPACE ${PACKAGE_NAME}::
    DESTINATION lib/cmake/${PACKAGE_NAME}
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"   # 这里使用PACKAGE_NAME变量
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/${PACKAGE_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
  INSTALL_DESTINATION lib/cmake/${PACKAGE_NAME}
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
  DESTINATION lib/cmake/${PACKAGE_NAME}
)

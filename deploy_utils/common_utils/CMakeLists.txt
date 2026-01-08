cmake_minimum_required(VERSION 3.8)
project(common_utils)

add_compile_options(-std=c++17)
add_compile_options(-O3)
set(CMAKE_CXX_STANDARD 17)

# create lib
set(source_file
  src/log.cpp
)

include_directories(
  include
)

add_library(${PROJECT_NAME} ${DEPLOY_LIB_TYPE} ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WINDOWS_EXPORT_ALL_SYMBOLS ON
	)
endif()

if (ENABLE_DEBUG_OUTPUT)
  target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_DEBUG_OUTPUT)
endif()

# install
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib)
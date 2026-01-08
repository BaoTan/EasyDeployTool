cmake_minimum_required(VERSION 3.8)
project(deploy_core)

add_compile_options(-std=c++17)
if(MSVC)
	add_compile_options(-O3 -fPIC)
else()
  add_compile_options(-O3 -Wextra -Wdeprecated -fPIC)
endif()


set(CMAKE_CXX_STANDARD 17)

# find libs 
find_package(OpenCV REQUIRED)
message("Deploy OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")

include_directories(
  include
  ${OpenCV_INCLUDE_DIRS}
)

set(source_file src/base_infer_core.cpp
                src/base_detection.cpp
                src/base_sam.cpp
                src/base_stereo.cpp
                src/base_mono_stereo.cpp
)

# create lib
add_library(${PROJECT_NAME} ${DEPLOY_LIB_TYPE} ${source_file})
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OpenCV_LIBS}
  common_utils
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WINDOWS_EXPORT_ALL_SYMBOLS ON
	)
endif()
# install
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib)



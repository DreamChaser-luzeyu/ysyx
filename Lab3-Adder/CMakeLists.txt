cmake_minimum_required(VERSION 3.25)

project(verilator_nvboard_demo_minimal)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/verilator_build/obj_dir)
include_directories(${PROJECT_SOURCE_DIR}/../../nvboard/include)
include_directories(/usr/share/verilator/include)

aux_source_directory(${PROJECT_SOURCE_DIR}/../../nvboard/src NVBOARD_SRCS)
message(${NVBOARD_SRCS})
set(
        SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/csrc/main.cpp
        ${PROJECT_SOURCE_DIR}/verilator_build/auto_bind.cpp
        ${PROJECT_SOURCE_DIR}/verilator_build/obj_dir/Vtop.cpp
        ${NVBOARD_SRCS}
)
add_executable(verilator_nvboard_demo_minimal ${SOURCE_FILES})
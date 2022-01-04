cmake_minimum_required(VERSION 3.16)

# Project setup
set(PROJECT_NAME "printf")
set(TARGET_NAME "app")
set(BIN_DIR ${CMAKE_SOURCE_DIR}/build/bin/${CMAKE_BUILD_TYPE})
project(${PROJECT_NAME})

######### TARGET RULES #########

# Main project files
set(INC_DIR code)
set(SRC_DIR code)
file(GLOB_RECURSE SRC_LIST
     "${SRC_DIR}/*.c"
	 "${INC_DIR}/*.h")

# Target setup
add_executable(${TARGET_NAME} ${SRC_LIST})
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${BIN_DIR})

# Includes
target_include_directories(${TARGET_NAME} PRIVATE ${INC_DIR})
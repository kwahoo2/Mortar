cmake_minimum_required(VERSION 3.16)

project(Mortar-Remote-Driver VERSION 0.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(PIGPIO_LIB pigpio REQUIRED)
find_library(RT_LIB rt REQUIRED)

add_compile_definitions(HASPIGPIO)

add_executable(${PROJECT_NAME}
    RemoteDriver.cpp
    MotorWorker.cpp
    ../CBoard.cpp
    ../CDigitalOutput.cpp
    ../CPWM.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Linkuj biblioteki
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PIGPIO_LIB}
    ${RT_LIB}
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

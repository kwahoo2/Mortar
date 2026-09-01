cmake_minimum_required(VERSION 3.16)

project(Mortar VERSION 0.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (SDL3 REQUIRED)
find_package(Qt6 QUIET COMPONENTS Core Gui Network Charts Widgets SerialPort Core5Compat)
find_package(Qt5 QUIET COMPONENTS Core Gui Network Charts Widgets SerialPort)

if(Qt6_FOUND)
    set(QT_VERSION 6)
    set(QT_CORE Qt6::Core)
    set(QT_GUI Qt6::Gui)
    set(QT_NETWORK Qt6::Network)
    set(QT_CHARTS Qt6::Charts)
    set(QT_WIDGETS Qt6::Widgets)
    set(QT_SERIALPORT Qt6::SerialPort)
    set(QT_CORE5COMPAT Qt6::Core5Compat)
elseif(Qt5_FOUND)
    set(QT_VERSION 5)
    set(QT_CORE Qt5::Core)
    set(QT_GUI Qt5::Gui)
    set(QT_NETWORK Qt5::Network)
    set(QT_CHARTS Qt5::Charts)
    set(QT_WIDGETS Qt5::Widgets)
    set(QT_SERIALPORT Qt5::SerialPort)
    set(QT_CORE5COMPAT)
else()
    message(FATAL_ERROR "Neither Qt5 nor Qt6 found! Please install Qt5 or Qt6.")
endif()

add_executable(${PROJECT_NAME}
    CorrectionTable.cpp
    GamepadInterface.cpp
    DataSender.cpp
    MotorWorker.cpp
    MotorDriver.cpp
    SerialDriver.cpp
    PrefsDialog.cpp
    StellarInterface.cpp
    main.cpp
    MainWindow.cpp

    CorrectionTable.h
    GamepadInterface.h
    MainWindow.h
    DataSender.h
    MotorWorker.h
    MotorDriver.h
    SerialDriver.h
    PrefsDialog.h
    StellarInterface.h

    CorrectionTable.ui
    MainWindow.ui
    PrefsDialog.ui

    resource.qrc
)

qt_wrap_cpp(MOC_SRCS
    CorrectionTable.h
    MainWindow.h
    PrefsDialog.h
    StellarInterface.h
    GamepadInterface.h
    DataSender.h
    MotorDriver.h
    MotorWorker.h
    SerialDriver.h
)

qt_wrap_ui(UI_HEADERS
    CorrectionTable.ui
    MainWindow.ui
    PrefsDialog.ui
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

qt_add_resources(RESOURCES resource.qrc)

target_sources(${PROJECT_NAME} PRIVATE ${UI_HEADERS} ${RESOURCES} ${MOC_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${QT_CORE}
    ${QT_GUI}
    ${QT_NETWORK}
    ${QT_CHARTS}
    ${QT_WIDGETS}
    ${QT_SERIALPORT}
    ${QT_CORE5COMPAT}
    SDL3::SDL3
)

find_library(XDO_LIB xdo)
if(XDO_LIB)
    add_compile_definitions(HASXDO)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${XDO_LIB})
else()
    message(WARNING "xdo not found, keyboard emulation will be disabled!")
endif()
find_library(PIGPIO_LIB pigpio)
if(PIGPIO_LIB)
    find_library(RT_LIB rt)
    if(RT_LIB)
        add_compile_definitions(HASPIGPIO)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${PIGPIO_LIB} ${RT_LIB})
    else()
        message(WARNING "librt not found, GPIO will be disabled!")
    endif()
    target_sources(${PROJECT_NAME} PRIVATE
        CBoard.cpp
        CDigitalOutput.cpp
        CPWM.cpp
    )
else()
    message(WARNING "pigpio not found, GPIO will be disabled!")
endif()

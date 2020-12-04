QT       += core gui network gamepad charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CorrectionTable.cpp \
    GamepadInterface.cpp \
    MotorDriver.cpp \
    MotorWorker.cpp \
    SerialDriver.cpp \
    PrefsDialog.cpp \
    StellarInterface.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    CorrectionTable.h \
    GamepadInterface.h \
    MainWindow.h \
    MotorDriver.h \
    MotorWorker.h \
    SerialDriver.h\
    PrefsDialog.h \
    StellarInterface.h

FORMS += \
    CorrectionTable.ui \
    MainWindow.ui \
    PrefsDialog.ui

LIBS += -lxdo

linux {
    contains(QMAKE_HOST.arch, arm.*):{
        LIBS += -lpigpio -lrt
        HEADERS += \
            CBoard.h \
            CDigitalOutput.h \
            CPWM.h
        SOURCES += \
            CBoard.cpp \
            CDigitalOutput.cpp \
            CPWM.cpp \
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

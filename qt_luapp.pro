TEMPLATE = app
CONFIG += console c++17
QT -= core gui app_bundle

SOURCES += \
	$$PWD/src/main.cpp

include(qt_luapp.pri)

DEFINES -= LUAPP_NO_MAIN_CPP

TEMPLATE = app
CONFIG += console c++11
QT -= core gui app_bundle

SOURCES += \
	$$PWD/src/main.cpp

include(qt_luapp.pri)
include(qt_lua.pri)

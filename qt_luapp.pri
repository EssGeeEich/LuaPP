INCLUDEPATH += $$PWD/include

SOURCES += \
	$$PWD/src/reference.cpp \
	$$PWD/src/state.cpp \
	$$PWD/src/typeext.cpp \
	$$PWD/src/variable.cpp

HEADERS += \
	$$PWD/include/enums.h \
	$$PWD/include/library.h \
	$$PWD/include/reference.h \
	$$PWD/include/state.h \
	$$PWD/include/transform.h \
	$$PWD/include/typeext.h \
	$$PWD/include/variable.h
	
LIBS += -llua
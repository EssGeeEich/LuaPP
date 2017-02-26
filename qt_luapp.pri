INCLUDEPATH += $$PWD/include

SOURCES += \
	$$PWD/src/reference.cpp \
	$$PWD/src/state.cpp \
	$$PWD/src/typeext.cpp \
	$$PWD/src/variable.cpp \
    $$PWD/src/library.cpp \
    $$PWD/src/util.cpp

HEADERS += \
	$$PWD/include/enums.h \
	$$PWD/include/library.h \
	$$PWD/include/reference.h \
	$$PWD/include/state.h \
	$$PWD/include/transform.h \
	$$PWD/include/variable.h \
    $$PWD/include/util.h \
    $$PWD/include/types.h \
    $$PWD/include/type_parser.h
	
LIBS += -llua
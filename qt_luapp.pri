include(qt_lua.pri)

INCLUDEPATH += $$PWD/include

SOURCES += \
	$$PWD/src/Functor.cpp \
	$$PWD/src/Reference.cpp \
	$$PWD/src/State.cpp \
	$$PWD/src/StateFunctions.cpp \
	$$PWD/src/StateManager.cpp \
	$$PWD/src/Utils.cpp

HEADERS += \
	$$PWD/include/Enums.hpp \
	$$PWD/include/Functor.hpp \
	$$PWD/include/FwdDecl.hpp \
	$$PWD/include/LuaInclude.hpp \
	$$PWD/include/LuaPP.hpp \
	$$PWD/include/MetatableManager.hpp \
	$$PWD/include/Reference.hpp \
	$$PWD/include/State.hpp \
	$$PWD/include/StateManager.hpp \
	$$PWD/include/Transform.hpp \
	$$PWD/include/TypeConverter.hpp \
	$$PWD/include/Utils.hpp
	

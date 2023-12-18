include(qt_lua.pri)

INCLUDEPATH += $$PWD/include

SOURCES += \
	$$PWD/src/LuaPP_Functor.cpp \
	$$PWD/src/LuaPP_Reference.cpp \
	$$PWD/src/LuaPP_State.cpp \
	$$PWD/src/LuaPP_StateFunctions.cpp \
	$$PWD/src/LuaPP_StateManager.cpp \
	$$PWD/src/LuaPP_Utils.cpp

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
	
DEFINES += LUAPP_NO_MAIN_CPP

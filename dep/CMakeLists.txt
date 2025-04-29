cmake_minimum_required(VERSION 3.16)
project(Lua)

# Includes
# To re-generate this list on an unix shell run:
# find * -type f -iname '*.h' -not -path '*testes*' -printf '${CMAKE_CURRENT_LIST_DIR}/%h/%f\n'
set(INCLUDE_FILES
	${CMAKE_CURRENT_LIST_DIR}/lua/lapi.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lauxlib.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lcode.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lctype.h
	${CMAKE_CURRENT_LIST_DIR}/lua/ldebug.h
	${CMAKE_CURRENT_LIST_DIR}/lua/ldo.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lfunc.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lgc.h
	${CMAKE_CURRENT_LIST_DIR}/lua/ljumptab.h
	${CMAKE_CURRENT_LIST_DIR}/lua/llex.h
	${CMAKE_CURRENT_LIST_DIR}/lua/llimits.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lmem.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lobject.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lopcodes.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lopnames.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lparser.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lprefix.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lstate.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lstring.h
	${CMAKE_CURRENT_LIST_DIR}/lua/ltable.h
	${CMAKE_CURRENT_LIST_DIR}/lua/ltests.h
	${CMAKE_CURRENT_LIST_DIR}/lua/ltm.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lua.h
	${CMAKE_CURRENT_LIST_DIR}/lua/luaconf.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lualib.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lundump.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lvm.h
	${CMAKE_CURRENT_LIST_DIR}/lua/lzio.h
)

# Sources
# To re-generate this list on an unix shell run:
# find * -type f -iname '*.c' -not -path '*testes*' -not -iname 'lua.c' -not -iname 'onelua.c' -printf '${CMAKE_CURRENT_LIST_DIR}/%h/%f\n'
set(SOURCE_FILES
	${CMAKE_CURRENT_LIST_DIR}/lua/lapi.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lauxlib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lbaselib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lcode.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lcorolib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lctype.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ldblib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ldebug.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ldo.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ldump.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lfunc.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lgc.c
	${CMAKE_CURRENT_LIST_DIR}/lua/linit.c
	${CMAKE_CURRENT_LIST_DIR}/lua/liolib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/llex.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lmathlib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lmem.c
	${CMAKE_CURRENT_LIST_DIR}/lua/loadlib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lobject.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lopcodes.c
	${CMAKE_CURRENT_LIST_DIR}/lua/loslib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lparser.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lstate.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lstring.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lstrlib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ltable.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ltablib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ltests.c
	${CMAKE_CURRENT_LIST_DIR}/lua/ltm.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lundump.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lutf8lib.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lvm.c
	${CMAKE_CURRENT_LIST_DIR}/lua/lzio.c
)

# Let cmake know it needs to compile them...
add_library(${PROJECT_NAME} STATIC
	${SOURCE_FILES}
	${INCLUDE_FILES}
)

# Add these include paths so that other sources can access this library...
# To re-generate this list on an unix shell run:
# find * -type f -iname '*.h' -not -path '*testes*' -printf '${CMAKE_CURRENT_LIST_DIR}/%h\n' | sort | uniq
target_include_directories(${PROJECT_NAME}
	PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lua
)

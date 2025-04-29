cmake_minimum_required(VERSION 3.19)
project(LuaPP)
set(CMAKE_CXX_STANDARD 17)

# Includes
# To re-generate this list on an unix shell run:
# find * -type f -iname '*.hpp' -printf '${CMAKE_CURRENT_LIST_DIR}/%h/%f\n'
set(INCLUDE_FILES
	${CMAKE_CURRENT_LIST_DIR}/include/Enums.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Functor.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/FwdDecl.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/LuaInclude.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/LuaPP.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/MetatableManager.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Reference.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/State.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/StateManager.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Transform.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/TypeConverter.hpp
	${CMAKE_CURRENT_LIST_DIR}/include/Utils.hpp
)

# Sources
# To re-generate this list on an unix shell run:
# find * -type f -iname '*.cpp' -printf '${CMAKE_CURRENT_LIST_DIR}/%h/%f\n'
set(SOURCE_FILES
	${CMAKE_CURRENT_LIST_DIR}/src/LuaPP_Functor.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/LuaPP_Reference.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/LuaPP_State.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/LuaPP_StateFunctions.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/LuaPP_StateManager.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/LuaPP_Utils.cpp
)

# Let cmake know it needs to compile them...
add_library(${PROJECT_NAME} STATIC
	${SOURCE_FILES}
	${INCLUDE_FILES}
)

# Include the dependencies...
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dep)
target_link_libraries(${PROJECT_NAME} PUBLIC
	Lua
)

# Add these include paths so that other sources can access this library...
# To re-generate this list on an unix shell run:
# find * -type f -iname '*.hpp' -printf '${CMAKE_CURRENT_LIST_DIR}/%h\n' | sort | uniq
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)


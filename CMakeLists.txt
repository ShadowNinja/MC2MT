cmake_minimum_required(VERSION 3.1)

project(MC2MT)

add_executable(${PROJECT_NAME}
	"${PROJECT_SOURCE_DIR}/src/main.cpp"
	"${PROJECT_SOURCE_DIR}/src/convert.cpp"
	"${PROJECT_SOURCE_DIR}/src/threads.cpp"
	"${PROJECT_SOURCE_DIR}/src/MCMap.cpp"
	"${PROJECT_SOURCE_DIR}/src/MTMap.cpp"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

add_subdirectory(nbt)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS "filesystem" "system")
find_package(SQLite3 REQUIRED)

target_link_libraries(${PROJECT_NAME}
	Threads::Threads
	"nbt"
	${ZLIB_LIBRARY}
	${SQLite3_LIBRARY}
	${Boost_LIBRARIES}
)
include_directories(
	"${NBT_SOURCE_DIR}/src"
	${ZLIB_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${SQLite3_INCLUDE_DIRS}
)

if (MINGW)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthreads")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
	COMPILE_FLAGS "-std=c++11 -Wall -Wextra -Wpedantic"
	RUNTIME_OUTPUT_DIRECTORY "bin")

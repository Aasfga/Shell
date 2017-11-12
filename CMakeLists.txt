cmake_minimum_required(VERSION 2.8.6)
project(Shell)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Tests)
set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
include_directories(include)
file(GLOB SOURCE_FILES src/*.c)
file(GLOB HEADERS include/*.h)

add_executable(Shell ${SOURCE_FILES} ${HEADERS} include/errors.h include/errors.h src/reader.c include/reader.h src/errors.c src/debug.c src/debug.h)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(Shell ${CMAKE_SOURCE_DIR}/lib/siparse.a)
else()
    target_link_libraries(Shell ${CMAKE_SOURCE_DIR}/lib/siparse.a)
endif()


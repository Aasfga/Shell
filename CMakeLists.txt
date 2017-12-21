cmake_minimum_required(VERSION 2.8.6)
project(shell)


set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
include_directories(include)
file(GLOB SOURCE_FILES src/*.c)
file(GLOB HEADERS include/*.h)

add_executable(shell ${SOURCE_FILES} ${HEADERS} src/shl_io.c include/shl_io.h src/shl_execute.c include/shl_execute.h src/shl_children.c include/shl_children.h)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(shell ${CMAKE_SOURCE_DIR}/lib/siparse.a)
else()
    target_link_libraries(shell ${CMAKE_SOURCE_DIR}/lib/siparse.a)
endif()


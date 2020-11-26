cmake_minimum_required(VERSION 3.0)

project("Word_and_Sword_server")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set ( PROJECT_LINK_LIBS  -lpng16 -lstdc++fs -lsfml-system -lsfml-network )

include_directories( "usr/include" "../shared/" "src/")

add_compile_options(-std=c++17 -Wall -Wextra -Wpedantic
#-Wconversion
#-Wcast-align
-Wunused
#-Wshadow
#-Wold-style-cast
#-Wpointer-arith -Wcast-qual -Wmissing-prototypes -Wno-missing-braces
)

file(GLOB SOURCES "src/*.cpp")
add_executable("Word_and_Sword_server" ${SOURCES})

target_link_libraries("Word_and_Sword_server" ${PROJECT_LINK_LIBS})

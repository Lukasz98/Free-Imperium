cmake_minimum_required(VERSION 3.0)

project("Word_and_Sword_client")


#SET(CMAKE_CXX_COMPILER /usr/bin/c++)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set ( PROJECT_LINK_LIBS  -lGL -lGLU -lglfw -lGLEW -lpng16 -lstdc++fs -lfreetype -lsfml-system -lsfml-network )

include_directories( "/usr/include" "/usr/include/freetype2" "../shared"  "src/" "../shared/" )

add_compile_definitions(HUMAN_PLAYER)
add_compile_options(-std=c++17 -O0 -g)
#add_compile_options(-std=c++17 -Wall -O0 -g)
#add_compile_options(-std=c++17 -Wall -Wextra -O0 -g)

file(GLOB SOURCES "src/*.cpp" "src/graphics/*.cpp" "src/gui/*.cpp" "src/gui_2/*.cpp" "src/gui_last/*.cpp" "../shared/shared_client/*.cpp" "../shared/*")
add_executable("Word_and_Sword_client" ${SOURCES})

target_link_libraries("Word_and_Sword_client" ${PROJECT_LINK_LIBS})

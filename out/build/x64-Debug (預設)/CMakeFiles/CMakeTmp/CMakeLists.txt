cmake_minimum_required(VERSION 3.17.20032601.0)
cmake_policy(SET CMP0091 OLD)
project(CMAKE_TRY_COMPILE C)
set(CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_DEFINITIONS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EXE_LINKER_FLAGS}")
include_directories(${INCLUDE_DIRECTORIES})
set(CMAKE_SUPPRESS_REGENERATION 1)
link_directories(${LINK_DIRECTORIES})
cmake_policy(SET CMP0065 OLD)
cmake_policy(SET CMP0083 OLD)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "D:/Practicum/CG/RollerCoasters/RollerCoasters/out/build/x64-Debug (預設)/CMakeFiles/CMakeTmp")
add_executable(cmTC_40090 "D:/Practicum/CG/RollerCoasters/RollerCoasters/out/build/x64-Debug (預設)/CMakeFiles/CMakeTmp/testCCompiler.c")
target_link_libraries(cmTC_40090 ${LINK_LIBRARIES})

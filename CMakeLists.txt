cmake_minimum_required(VERSION 3.7...3.24)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(
    Debugger
    VERSION 1.0
    LANGUAGES CXX)

# add_executable(one two.cpp three.h)
add_executable(debug src/debug.cxx extern/linenoise/linenoise.c)
target_compile_features(debug PUBLIC cxx_std_20)
set_target_properties(debug PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(debug PUBLIC include extern/libelfin extern/linenoise)

add_custom_target(
    libelfin
    COMMAND make
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/extern/libelfin)

# target_link_libraries(debug
#     ${PROJECT_SOURCE_DIR}/extern/libelfin/dwarf/libdwarf++.so
#     ${PROJECT_SOURCE_DIR}/extern/libelfin/elf/libelf++.so)
# add_dependencies(debug libelfin)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,modernize-*,hicpp-*,performance-*,clang-analyzer-*")
set_target_properties(debug PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
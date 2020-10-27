cmake_minimum_required(VERSION 3.02)
project(WindowsKeyMapper VERSION 0.1)

if (NOT WIN32)
    message(FATAL_ERROR "Operating system not supported.")
elseif(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "Compiler not supported.")
elseif(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION STREQUAL "")
    message(FATAL_ERROR "No Windows SDK found.")
endif()

add_executable(WindowsKeyMapper src/WindowsKeyMapper.cpp)
target_compile_options(WindowsKeyMapper PRIVATE /EHsc /W4 /WX)
target_link_libraries(WindowsKeyMapper PRIVATE user32.lib)

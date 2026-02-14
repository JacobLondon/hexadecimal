cmake_minimum_required(VERSION 3.10)

project(hd LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(hd
    hd.cpp
    util.cpp
    rpn_include.cpp
)

set_target_properties(hd PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_options(hd PRIVATE
    -Wall
    -Wextra
    -Wno-switch
    -Wno-ignored-qualifiers
    -pipe
)

# Match Makefile per-OS flags
if(WIN32)
    target_compile_options(hd PRIVATE -Wno-cast-function-type)
elseif(APPLE)
    # no extra flags
else()
    target_compile_options(hd PRIVATE -Wno-cast-function-type)
    target_link_libraries(hd PRIVATE m)
endif()

# Match Makefile debug/release behavior
#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    target_compile_options(hd PRIVATE -ggdb -O0)
#elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#    target_compile_options(hd PRIVATE -O2)
#endif()

target_compile_options(hd PRIVATE -stdlib=libc++)

install(TARGETS hd RUNTIME DESTINATION bin)

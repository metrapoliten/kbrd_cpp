cmake_minimum_required(VERSION 3.27)
project(backlight CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE sources
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(backlight ${sources})

target_include_directories(backlight PUBLIC "${CMAKE_SOURCE_DIR}/include")

target_compile_options(backlight PRIVATE
        -Wall -Wextra -pedantic-errors -Wsign-conversion -Wconversion -Werror -Weffc++ -Winline)

set(HIDAPI_WITH_LIBUSB FALSE)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(hidapi)

target_link_libraries(backlight PRIVATE hidapi::hidapi)

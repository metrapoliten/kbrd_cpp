cmake_minimum_required(VERSION 3.27)
project(backlight CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRCS
    src/impl/ControllerImpl.cpp
    src/impl/ModelImpl.cpp
    src/impl/ViewImpl.cpp
    src/Controller.cpp
    src/Model.cpp
    src/View.cpp
)

add_executable(backlight src/main.cpp)
target_sources(backlight
    PRIVATE src/main.cpp ${SRCS}
    PRIVATE
        FILE_SET HEADERS
        BASE_DIRS include
)

target_compile_options(backlight PRIVATE
        -Wall -Wextra -pedantic-errors -Wsign-conversion -Wconversion -Werror -Weffc++ -Winline)

set(HIDAPI_WITH_LIBUSB FALSE)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(hidapi)

target_link_libraries(backlight PRIVATE hidapi::hidapi)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(backlight.test)
target_sources(backlight.test
    PRIVATE test/FakeHidapi.cpp test/ControllerImplTest.cpp ${SRCS}
    PRIVATE
        FILE_SET HEADERS
        BASE_DIRS
            include
            test
)

target_link_libraries(backlight.test PRIVATE GTest::gtest_main GTest::gmock hidapi::include)

include(GoogleTest)
gtest_discover_tests(backlight.test)

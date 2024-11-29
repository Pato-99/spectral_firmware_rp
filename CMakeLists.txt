cmake_minimum_required(VERSION 3.27)
#set(CMAKE_C_STANDARD 23)

#set(PICO_PLATFORM rp2350)
set(PICO_BOARD pico2)

include(pico-sdk/pico_sdk_init.cmake)

project(avcr_spectral_fw C CXX ASM)

pico_sdk_init()

add_subdirectory(pico-accelstepper)
add_subdirectory(mylib)

#add_subdirectory(test)


add_executable(fw main.cpp interrupts.cpp)
target_link_libraries(fw
        mylib
        pico_stdlib
        accelstepper
)

pico_add_uf2_output(fw)
pico_enable_stdio_usb(fw 1)
pico_enable_stdio_uart(fw 0)

# test
add_executable(tmctest test/tmctest.cpp)
target_link_libraries(tmctest
        mylib
        pico_stdlib
)
pico_add_uf2_output(tmctest)
pico_enable_stdio_usb(tmctest 1)
pico_enable_stdio_uart(tmctest 0)



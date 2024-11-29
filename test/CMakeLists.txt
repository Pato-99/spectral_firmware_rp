
add_executable(tmctest tmctest.cpp)
target_link_libraries(tmctest
        mylib
        pico_stdlib
)
pico_add_uf2_output(tmctest)
pico_enable_stdio_usb(tmctest 1)
pico_enable_stdio_uart(tmctest 0)

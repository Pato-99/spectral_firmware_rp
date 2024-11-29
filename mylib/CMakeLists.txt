add_library(mylib
    src/controller.cpp
        src/relay.cpp
        src/mg90s.cpp
        src/tmccom.cpp
        src/myaccelstepper.cpp
        src/controller.cpp
)


message(${CMAKE_CURRENT_LIST_DIR})

target_include_directories(mylib PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/inc"
)

target_link_libraries(mylib
        pico_stdlib
        hardware_pwm
        accelstepper
)

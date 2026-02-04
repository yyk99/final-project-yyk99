#
#
#

project(iot_in_c)
cmake_minimum_required(VERSION 3.13)

macro (add_page name)
	add_executable(${name} ${name}.c)
	target_link_libraries(${name} -lbcm2835)
	target_link_libraries(${name} utils)
endmacro(add_page)

add_library(utils STATIC
    temperature_utils.c
    temperature_utils.h
    blinker.c
    blinker.h
    )


add_page(p1)

add_page(page57)

add_page(page83)

add_page(page103)

add_page(page195)

add_page(page195_dht11)

add_page(page195_dht11_srv)

add_page(blinker_test)

install(TARGETS
    page195_dht11_srv page195_dht11 blinker_test
    CONFIGURATIONS Release
    DESTINATION bin
)

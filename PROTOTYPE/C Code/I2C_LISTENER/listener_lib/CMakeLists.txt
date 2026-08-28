add_library(listener_lib listener_lib.cpp)
target_include_directories(listener_lib PUBLIC .)
target_link_libraries(listener_lib PUBLIC pico_stdlib)
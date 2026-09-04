add_library(target_lib target_lib.cpp)
target_include_directories(target_lib PUBLIC .)
target_link_libraries(target_lib PUBLIC pico_stdlib)
add_executable(main main.cpp)

target_include_directories(main PUBLIC .)
target_link_libraries(main PRIVATE sndfile)

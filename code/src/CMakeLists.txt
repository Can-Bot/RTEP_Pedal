# Define the main pedal library
add_library(pedal_lib Sample.cpp DigitalSignalChain.cpp Harmonizer.cpp memory-tracker.cpp)

# Ensure the library is linked correctly
target_include_directories(pedal_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(pedal_lib PRIVATE pthread PRIVATE sndfile)

# Define `shred_pedal` executable
add_executable(shred_pedal main.cpp)
target_link_libraries(shred_pedal PRIVATE pedal_lib stdc++fs)

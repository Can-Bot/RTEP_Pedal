# Define the main pedal library
add_library(pedal_lib DigitalSignalChain.cpp)

# Ensure the library is linked correctly
target_include_directories(pedal_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(pedal_lib PRIVATE pthread)

# Define `harmoniser_pedal` executable
add_executable(harmoniser_pedal main.cpp)
target_link_libraries(harmoniser_pedal PRIVATE pedal_lib)
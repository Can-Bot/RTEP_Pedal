# src/CMakeLists.txt

# Collect all .cpp files in subdirectories (except test)
file(GLOB SRC_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB DISPLAY_SOURCES "${CMAKE_SOURCE_DIR}/src/display/*.cpp")
file(GLOB DSP_SOURCES "${CMAKE_SOURCE_DIR}/src/dsp/*.cpp")
file(GLOB SAMPLING_SOURCES "${CMAKE_SOURCE_DIR}/src/sampling/*.cpp")
file(GLOB EFFECT_SOURCES "${CMAKE_SOURCE_DIR}/src/effects/*.cpp")

# Combine all source files
set(ALL_SOURCES
    ${SRC_SOURCES}
    ${DISPLAY_SOURCES}
    ${DSP_SOURCES}
    ${SAMPLING_SOURCES}
    ${EFFECT_SOURCES}
)

# Remove main.cpp so it isn't part of the library
list(REMOVE_ITEM ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# Define the main pedal library
add_library(pedal_lib ${ALL_SOURCES})

# Include directories
target_include_directories(pedal_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/display
    ${CMAKE_SOURCE_DIR}/src/dsp
    ${CMAKE_SOURCE_DIR}/src/sampling
    ${CMAKE_SOURCE_DIR}/src/effects
)

# Link required libraries
target_link_libraries(pedal_lib PRIVATE pthread sndfile)

# Main executable
add_executable(shred_pedal main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/dsp/ForceEffects.cpp)
target_link_libraries(shred_pedal PRIVATE pedal_lib stdc++fs)


# Add tests (integration + unit)
add_subdirectory(test)

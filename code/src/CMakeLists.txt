#src/CMakeLists.txt
# Macro definition to force use of lgpio library by display driver
add_definitions(-DUSE_DEV_LIB)

# --- Collect all relevant source files in subdirectories ---

# Top-level .cpp files (excluding main.cpp)
file(GLOB SRC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# UI-related modules
file(GLOB UI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.cpp")

# DSP (Digital Signal Processing) code, including ForceEffects.cpp
file(GLOB DSP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dsp/*.cpp")

# Options and Configuration Logic
file(GLOB CFG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/options/*.cpp")

# Sampling logic (e.g. audio input)
file(GLOB SAMPLING_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sampling/*.cpp")

# Audio effects, each typically registers itself with the factory
file(GLOB EFFECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/effects/*.cpp")

# Encoder Input and Handler
file(GLOB INPUT_SOURCES "${CMAKE_SOURCE_DIR}/src/encoder_input/*.cpp")
file(GLOB GPIO_SOURCES "${CMAKE_SOURCE_DIR}/src/gpio_event/*.cpp")


# --- Exclude main.cpp from the library (linked separately in executable) ---
list(REMOVE_ITEM SRC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")


# --- Combine all source files into one list ---
set(ALL_SOURCES
    ${SRC_SOURCES}
    ${UI_SOURCES}
    ${DSP_SOURCES}
    ${CFG_SOURCES}
    ${SAMPLING_SOURCES}
    ${EFFECT_SOURCES}
    ${INPUT_SOURCES}
    ${GPIO_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp/ForceEffects.cpp  # Ensures all effects are linked
)


# --- Define the main library for the pedal ---
# This library includes all core modules, ui, dsp, effects, etc.
add_library(pedal_lib ${ALL_SOURCES})



# --- Include directories so headers can be found when linking this library ---
target_include_directories(pedal_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/server
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/options
    ${CMAKE_CURRENT_SOURCE_DIR}/dsp
    ${CMAKE_CURRENT_SOURCE_DIR}/sampling
    ${CMAKE_CURRENT_SOURCE_DIR}/effects
    ${CMAKE_SOURCE_DIR}/src/encoder_input
    ${CMAKE_SOURCE_DIR}/src/gpio_event
)


# --- Link third-party dependencies used by this library ---
# pthread: multithreading
# sndfile: audio file I/O
# lgpio: GPIO library
target_link_libraries(pedal_lib PRIVATE pthread ${SNDFILE_LIBRARIES} ${ALSA_LIBRARIES} lgpio gpiod)
# --- Define the main application executable ---
# This is the real-time pedal program that runs on the Pi
add_executable(shred_pedal main.cpp)

# Include ALSA and SNDFILE headers for main if needed
target_include_directories(shred_pedal PRIVATE ${ALSA_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS})

# Link the main executable with pedal_lib and filesystem
target_link_libraries(shred_pedal PRIVATE pedal_lib stdc++fs ${SNDFILE_LIBRARIES} ${ALSA_LIBRARIES})


# --- Include and build the test directory ---
# This will build func_test and any unit tests defined
add_subdirectory(test)
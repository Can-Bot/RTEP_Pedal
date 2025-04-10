#code/CmMakeLists.txt
cmake_minimum_required(VERSION 3.7.0)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -pedantic -Werror)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")

project(shred_pedal VERSION 1.0 LANGUAGES CXX)

# Find ALSA and libsndfile packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)

# Try finding libsndfile with pkg-config first
pkg_check_modules(SNDFILE QUIET sndfile)

if(NOT SNDFILE_FOUND)
    # If pkg-config fails, try finding it manually
    find_library(SNDFILE_LIBRARY NAMES sndfile)
    find_path(SNDFILE_INCLUDE_DIR NAMES sndfile.h PATH_SUFFIXES sndfile)
    
    if(SNDFILE_LIBRARY AND SNDFILE_INCLUDE_DIR)
        set(SNDFILE_FOUND TRUE)
        set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
        set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "libsndfile not found. Install it and ensure pkg-config can find it.")
    endif()
endif()

enable_testing()

# Add the `src/` directory where the real targets are defined
add_subdirectory(src)

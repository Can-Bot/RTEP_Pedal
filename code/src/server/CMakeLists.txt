# server/CMakeLists.txt

add_custom_target(usockets_build
    COMMAND make
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/uSockets
    COMMENT "Building uSockets (libuSockets.a)"
)

# Create an imported target for the built static lib
add_library(uSockets STATIC IMPORTED GLOBAL)
set_target_properties(uSockets PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/uSockets/uSockets.a
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/lib/uSockets/src
)

add_dependencies(uSockets usockets_build)

# Now define your executable
add_executable(shred_server shred_server.cpp)

target_include_directories(shred_server PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/vcpkg/installed/arm64-linux/include
)

target_link_libraries(shred_server PRIVATE
    uSockets
    ssl
    crypto
    z
    pthread
)

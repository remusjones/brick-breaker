add_subdirectory(SDL)
add_subdirectory(HelloECS)

add_library(vendored INTERFACE)

target_link_libraries(vendored INTERFACE HelloECS SDL3::SDL3)
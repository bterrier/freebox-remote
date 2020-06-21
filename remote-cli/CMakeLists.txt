add_executable(remote-cli
    src/mapping.c
    src/mapping.h
    src/remote.c
    src/term_input.c
    src/term_input.h)

target_link_libraries(remote-cli elaqt foils_hid rudp)

target_link_directories(remote-cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../build/lib)

target_include_directories(remote-cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../libela/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../librudp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../foils_hid/include)

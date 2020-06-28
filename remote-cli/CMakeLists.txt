add_executable(remote-cli
    src/mapping.c
    src/mapping.h
    src/remote.c
    src/term_input.c
    src/term_input.h)

target_link_libraries(remote-cli elaqt foils_hid rudp ela rudp foils_hid)

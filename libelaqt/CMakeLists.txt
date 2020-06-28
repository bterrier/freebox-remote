find_package(Qt5 COMPONENTS Core REQUIRED)

add_library(elaqt
    include/elaqt/elaqt.h
    src/elaqt.cpp)

target_include_directories(elaqt PUBLIC
    include)


target_link_libraries(elaqt PUBLIC Qt5::Core ela)




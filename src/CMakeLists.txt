add_library(CLua
    value.cpp
    )
target_link_libraries(CLua PUBLIC Flags)

add_executable(cscheme
    cscheme.cpp
    )
target_link_libraries(cscheme PUBLIC
    Flags
    CLua
    )

# src/CMakeLists.txt

add_custom_command(OUTPUT lex.cpp
    COMMAND re2c -o lex.cpp lex.re
    MAIN_DEPENDENCY lex.re
    )

add_executable(cscheme lex.cpp cscheme.cpp)
target_link_libraries(cscheme PUBLIC Flags)

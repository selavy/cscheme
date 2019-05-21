# src/CMakeLists.txt

add_custom_command(
    OUTPUT lex.cpp
    COMMAND re2c -o ${CMAKE_CURRENT_BINARY_DIR}/lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lex.re
    DEPENDS lex.re
    )
add_library(Lexer lex.cpp)
target_include_directories(Lexer INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

add_executable(cscheme
    cscheme.cpp
    value.h
    )
target_link_libraries(cscheme PUBLIC Flags Lexer)

add_custom_command(
    OUTPUT lex.h
    COMMAND re2c -o ${CMAKE_CURRENT_BINARY_DIR}/lex.h ${CMAKE_CURRENT_SOURCE_DIR}/lex.re
    DEPENDS lex.re
    )
add_library(Lexer lex.h value.cpp)
target_include_directories(Lexer PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(Lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Lexer PUBLIC Flags)

add_executable(cscheme cscheme.cpp)
target_link_libraries(cscheme PUBLIC Flags Lexer)

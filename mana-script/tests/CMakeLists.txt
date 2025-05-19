# Add tests
add_executable(test_lexer ../src/lexer.cpp ../src/token.cpp ../src/error.cpp ../src/symbol_table.cpp ../src/ast.cpp test_lexer.cpp)

# Add tests to CTest
add_test(NAME LexerTest COMMAND test_lexer)
# CMake generated Testfile for 
# Source directory: C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests
# Build directory: C:/Users/hp/Desktop/New folder/mana-script/mana-script/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(LexerTest "C:/Users/hp/Desktop/New folder/mana-script/mana-script/build/tests/Debug/test_lexer.exe")
  set_tests_properties(LexerTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;5;add_test;C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(LexerTest "C:/Users/hp/Desktop/New folder/mana-script/mana-script/build/tests/Release/test_lexer.exe")
  set_tests_properties(LexerTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;5;add_test;C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(LexerTest "C:/Users/hp/Desktop/New folder/mana-script/mana-script/build/tests/MinSizeRel/test_lexer.exe")
  set_tests_properties(LexerTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;5;add_test;C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(LexerTest "C:/Users/hp/Desktop/New folder/mana-script/mana-script/build/tests/RelWithDebInfo/test_lexer.exe")
  set_tests_properties(LexerTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;5;add_test;C:/Users/hp/Desktop/New folder/mana-script/mana-script/tests/CMakeLists.txt;0;")
else()
  add_test(LexerTest NOT_AVAILABLE)
endif()

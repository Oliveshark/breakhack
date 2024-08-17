# CMake generated Testfile for 
# Source directory: /home/linus/ws/breakhack
# Build directory: /home/linus/ws/breakhack/build/release
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_util "test_util")
set_tests_properties(test_util PROPERTIES  _BACKTRACE_TRIPLES "/home/linus/ws/breakhack/CMakeLists.txt;288;add_test;/home/linus/ws/breakhack/CMakeLists.txt;0;")
add_test(test_linkedlist "test_linkedlist")
set_tests_properties(test_linkedlist PROPERTIES  _BACKTRACE_TRIPLES "/home/linus/ws/breakhack/CMakeLists.txt;292;add_test;/home/linus/ws/breakhack/CMakeLists.txt;0;")
add_test(test_hashtable "test_hashtable")
set_tests_properties(test_hashtable PROPERTIES  _BACKTRACE_TRIPLES "/home/linus/ws/breakhack/CMakeLists.txt;296;add_test;/home/linus/ws/breakhack/CMakeLists.txt;0;")
add_test(test_input "test_input")
set_tests_properties(test_input PROPERTIES  _BACKTRACE_TRIPLES "/home/linus/ws/breakhack/CMakeLists.txt;306;add_test;/home/linus/ws/breakhack/CMakeLists.txt;0;")
subdirs("lib/bh_random")
subdirs("lib/checksum")
subdirs("lib/physfs-3.0")

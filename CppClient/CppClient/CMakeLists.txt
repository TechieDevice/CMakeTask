cmake_minimum_required (VERSION 3.8)

find_package(Threads REQUIRED)
add_library(CppClient_library STATIC "CppClient.cpp" "CppClient.h")
add_executable (CppClient_main "Reader.cpp")
target_link_libraries(CppClient_main CppClient_library Threads::Threads)	

cmake_minimum_required (VERSION 3.8)

add_library(CppServer_library STATIC "CppServer.cpp" "CppServer.h")
add_executable (CppServer_main "Receiver.cpp")
target_link_libraries(CppServer_main CppServer_library)

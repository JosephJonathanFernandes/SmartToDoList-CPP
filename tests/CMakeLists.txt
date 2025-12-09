cmake_minimum_required(VERSION 3.15)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.10
)
FetchContent_MakeAvailable(Catch2)

add_executable(test_todo test_todo.cpp)
target_link_libraries(test_todo PRIVATE Catch2::Catch2 smarttodo_lib)
include(CTest)
include(Catch)
catch_discover_tests(test_todo)

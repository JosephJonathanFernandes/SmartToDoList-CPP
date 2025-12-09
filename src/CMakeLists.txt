add_library(smarttodo_lib
    todo.cpp
)

target_include_directories(smarttodo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(smarttodo_app
    main.cpp
)

target_link_libraries(smarttodo_app PRIVATE smarttodo_lib)

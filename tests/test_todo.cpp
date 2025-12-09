#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../include/todo.h"

TEST_CASE("insert and peek and remove") {
    smarttodo::ToDoList list;
    list.insertTask(2, "Test task 1", "");
    list.insertTask(1, "Urgent task", "");
    // peek should not throw
    REQUIRE_NOTHROW(list.peekTask());
    // remove two tasks
    list.removeTask();
    list.removeTask();
}

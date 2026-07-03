#include <memory>
#include <print>
#define private public
#define protected public
// sneaky ^

#include "TodoList.hpp"

#include <assert.h>

//---- test cases:
void test_add_task(){
    TodoList todo_list;
    todo_list.addTask((std::make_unique<SimpleTask>("task 1")));
    todo_list.addTask((std::make_unique<ImportantTask>("task 2")));
    todo_list.addTask((std::make_unique<Task>("task 3")));

    assert(todo_list.size() == 3);
    assert(todo_list.front()->m_description == "task 1");
    todo_list.remove_first();

    assert(todo_list.size() == 2);
    assert(todo_list.front()->m_description == "task 2");
    todo_list.remove_first();

    assert(todo_list.size() == 1);
    assert(todo_list.front()->m_description == "task 3");
    todo_list.remove_first();

    assert(todo_list.size() == 0);
    std::println("Passed basic Unit Tests");
}

int main(){

    test_add_task();
}
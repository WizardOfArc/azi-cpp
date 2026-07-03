#pragma once

#include <print>
#include <string>
#include <memory>
#include <list>

class Task {
public:
   explicit Task(std::string description) : m_description(std::move(description)){}
   virtual ~Task() = default;
   virtual void display() const {
      std::println("Task: {}", m_description);
   }
protected:
    std::string m_description;
};

class SimpleTask : public Task {
public:
    explicit SimpleTask(std::string desc) : Task(desc){}

   void display() const override {
      std::println("[Simple] Task: {}", m_description);
   }
};

class ImportantTask : public Task {
public:
    explicit ImportantTask(std::string desc) : Task(desc){}

   void display() const override {
      std::println("[Important] Task: {}", m_description);
   }
};

class TodoList : public std::list<std::unique_ptr<Task>> {
public:
    void display_tasks() const {
        for(const auto& task: *this){
            task->display();
        }
    }    

    void addTask(std::unique_ptr<Task> task){
       this->push_back(std::move(task));
    }

    void remove_first() {
        this->pop_front();
    }
};

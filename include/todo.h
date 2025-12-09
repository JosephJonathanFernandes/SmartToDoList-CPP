#pragma once
#include <string>
#include <vector>

namespace smarttodo {

struct Task {
    int priority;
    std::string description;
    std::string timestamp;
    std::string dueDate;
};

class ToDoList {
public:
    ToDoList() = default;
    void insertTask(int priority, const std::string& desc, const std::string& dueDate);
    void removeTask();
    void peekTask() const;
    void displayTasks() const;
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    void exportToCSV(const std::string& filename) const;
    void remindUrgentTasks() const;

private:
    std::vector<Task> tasks_;
};

} // namespace smarttodo

#include "../include/todo.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace smarttodo {

static const int MAX_TASKS = 1000; // increased limit for flexibility

static std::string getCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm* ltm = std::localtime(&now);
    std::ostringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << std::setw(2) << std::setfill('0') << 1 + ltm->tm_mon << "-"
       << std::setw(2) << std::setfill('0') << ltm->tm_mday << " "
       << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
       << std::setw(2) << std::setfill('0') << ltm->tm_min;
    return ss.str();
}

static bool parseDateTime(const std::string& dateStr, std::tm& dateTm) {
    std::istringstream ss(dateStr);
    ss >> std::get_time(&dateTm, "%Y-%m-%d %H:%M");
    return !ss.fail();
}

static bool isOverdue(const std::string& dueDate) {
    if (dueDate.empty()) return false;
    std::tm dueTm = {};
    if (!parseDateTime(dueDate, dueTm)) return false;
    std::time_t now = std::time(nullptr);
    std::time_t dueTime = std::mktime(&dueTm);
    return std::difftime(now, dueTime) > 0;
}

static bool isDueSoon(const std::string& dueDate, int hoursAhead = 24) {
    if (dueDate.empty()) return false;
    std::tm dueTm = {};
    if (!parseDateTime(dueDate, dueTm)) return false;
    std::time_t now = std::time(nullptr);
    std::time_t dueTime = std::mktime(&dueTm);
    double diffSeconds = std::difftime(dueTime, now);
    return diffSeconds > 0 && diffSeconds <= hoursAhead * 3600;
}

static void escapeCSV(std::string& s) {
    size_t pos = 0;
    while ((pos = s.find('"', pos)) != std::string::npos) {
        s.replace(pos, 1, """"");
        pos += 2;
    }
}

// comparator for min-heap by priority (lower value = higher priority)
static bool taskCompare(const Task& a, const Task& b) {
    return a.priority > b.priority; // std::push_heap expects max-heap comparator
}

void ToDoList::insertTask(int priority, const std::string& desc, const std::string& dueDate) {
    if (static_cast<int>(tasks_.size()) >= MAX_TASKS) {
        std::cerr << "Task list full!" << std::endl;
        return;
    }

    Task t{priority, desc, getCurrentDateTime(), dueDate};
    tasks_.push_back(t);
    std::push_heap(tasks_.begin(), tasks_.end(), taskCompare);
    std::cout << "Task added at " << t.timestamp << std::endl;
}

void ToDoList::removeTask() {
    if (tasks_.empty()) {
        std::cout << "No tasks to remove!" << std::endl;
        return;
    }

    // top is tasks_.front() because we maintain heap
    std::pop_heap(tasks_.begin(), tasks_.end(), taskCompare);
    Task t = tasks_.back();
    tasks_.pop_back();
    std::cout << "Completed Task: " << t.description << " (Added: " << t.timestamp << ")" << std::endl;
}

void ToDoList::peekTask() const {
    if (tasks_.empty()) {
        std::cout << "No tasks available!" << std::endl;
        return;
    }
    const Task& t = tasks_.front();
    std::cout << "Next Task: " << t.description << " (Priority " << t.priority << ", Added: " << t.timestamp << ", Due: " << (t.dueDate.empty() ? "None" : t.dueDate) << ")" << std::endl;
}

void ToDoList::displayTasks() const {
    if (tasks_.empty()) {
        std::cout << "No tasks available!" << std::endl;
        return;
    }

    std::cout << "\nYour To-Do List (Heap View):\n";
    std::cout << "---------------------------------------------------------------" << std::endl;

    // For display only, copy into temporary vector and sort by priority ascending
    std::vector<Task> copy = tasks_;
    std::sort(copy.begin(), copy.end(), [](const Task& a, const Task& b){ return a.priority < b.priority; });

    for (const auto& task : copy) {
        std::string status = "";
        if (isOverdue(task.dueDate)) status = " (OVERDUE)";
        else if (isDueSoon(task.dueDate)) status = " (Due Soon)";

        std::cout << "Priority: " << task.priority
                  << " | Added: " << task.timestamp
                  << " | Due: " << (task.dueDate.empty() ? "None" : task.dueDate)
                  << status
                  << " | Task: " << task.description << std::endl;
    }

    std::cout << "---------------------------------------------------------------" << std::endl;
}

void ToDoList::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) return;
    // write linearized list (not a heap order guarantee)
    for (const auto& t : tasks_) {
        file << t.priority << "|" << t.timestamp << "|" << t.dueDate << "|" << t.description << "\n";
    }
}

void ToDoList::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return;
    tasks_.clear();
    std::string line;
    while (std::getline(file, line)) {
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);
        size_t pos3 = line.find('|', pos2 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) continue;
        int prio = 0;
        try { prio = std::stoi(line.substr(0, pos1)); } catch(...) { continue; }
        std::string ts = line.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string due = line.substr(pos2 + 1, pos3 - pos2 - 1);
        std::string desc = line.substr(pos3 + 1);
        Task t{prio, desc, ts, due};
        tasks_.push_back(t);
    }
    if (!tasks_.empty()) std::make_heap(tasks_.begin(), tasks_.end(), taskCompare);
}

void ToDoList::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to write CSV" << std::endl;
        return;
    }
    file << "Priority,Added,Due Date,Description\n";
    for (const auto& t : tasks_) {
        std::string desc = t.description;
        escapeCSV(desc);
        file << t.priority << ",\"" << t.timestamp << "\",\"" << t.dueDate << "\",\"" << desc << "\"\n";
    }
    std::cout << "Tasks exported to " << filename << std::endl;
}

void ToDoList::remindUrgentTasks() const {
    bool hasUrgent = false;
    for (const auto& t : tasks_) {
        if (isOverdue(t.dueDate)) {
            std::cout << "Overdue Task: " << t.description << std::endl;
            hasUrgent = true;
        } else if (isDueSoon(t.dueDate)) {
            std::cout << "Due Soon: " << t.description << " due by " << t.dueDate << std::endl;
            hasUrgent = true;
        }
    }
    if (!hasUrgent) std::cout << "No urgent tasks at the moment." << std::endl;
}

} // namespace smarttodo

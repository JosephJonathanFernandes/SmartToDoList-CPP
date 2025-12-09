#include "../include/todo.h"

#include <iostream>
#include <limits>

int main() {
    smarttodo::ToDoList toDoList;
    const std::string dataFilename = "tasks.txt";
    const std::string csvFilename = "tasks_export.csv";

    toDoList.loadFromFile(dataFilename);
    std::cout << "\n--- Task Reminders on Startup ---\n";
    toDoList.remindUrgentTasks();

    char choice{};
    int priority{};
    std::string description;
    std::string dueDate;

    do {
        std::cout << "\nSmart To-Do List\n";
        std::cout << "a. Add Task\n"
                  << "r. Remove Task\n"
                  << "v. View Tasks\n"
                  << "p. Peek Task\n"
                  << "x. Export Tasks to CSV\n"
                  << "e. Exit\n"
                  << "Enter your choice: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 'a': case 'A':
                std::cout << "Enter Task Description: ";
                std::getline(std::cin, description);
                std::cout << "Enter Priority (1-5, 1 = Highest): ";
                if (!(std::cin >> priority)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid priority\n";
                    break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (priority < 1 || priority > 5) {
                    std::cout << "Invalid priority! Please enter 1 to 5.\n";
                    break;
                }
                std::cout << "Enter Due Date and Time (YYYY-MM-DD HH:MM) or leave empty: ";
                std::getline(std::cin, dueDate);
                toDoList.insertTask(priority, description, dueDate);
                break;

            case 'r': case 'R':
                toDoList.removeTask();
                break;

            case 'v': case 'V':
                toDoList.displayTasks();
                break;

            case 'p': case 'P':
                toDoList.peekTask();
                break;

            case 'x': case 'X':
                toDoList.exportToCSV(csvFilename);
                break;

            case 'e': case 'E':
                toDoList.saveToFile(dataFilename);
                std::cout << "Tasks saved. Goodbye!\n";
                break;

            default:
                std::cout << "Invalid option. Try again.\n";
        }

    } while (choice != 'e' && choice != 'E');

    return 0;
}

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

#define MAX_TASKS 100

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string CYAN = "\033[36m";

string getCurrentDateTime() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << setw(2) << setfill('0') << 1 + ltm->tm_mon << "-"
       << setw(2) << setfill('0') << ltm->tm_mday << " "
       << setw(2) << setfill('0') << ltm->tm_hour << ":"
       << setw(2) << setfill('0') << ltm->tm_min << ":"
       << setw(2) << setfill('0') << ltm->tm_sec;
    return ss.str();
}

// Parse date string "YYYY-MM-DD HH:MM" into tm struct
bool parseDateTime(const string& dateStr, tm& dateTm) {
    istringstream ss(dateStr);
    ss >> get_time(&dateTm, "%Y-%m-%d %H:%M");
    return !ss.fail();
}

// Check if current time > due date (i.e., overdue)
bool isOverdue(const string& dueDate) {
    if (dueDate.empty()) return false;

    tm dueTm = {};
    if (!parseDateTime(dueDate, dueTm)) return false;

    time_t now = time(nullptr);
    time_t dueTime = mktime(&dueTm);
    return difftime(now, dueTime) > 0;  // positive if now > dueTime
}

// Check if due within next N hours (default 24)
bool isDueSoon(const string& dueDate, int hoursAhead = 24) {
    if (dueDate.empty()) return false;

    tm dueTm = {};
    if (!parseDateTime(dueDate, dueTm)) return false;

    time_t now = time(nullptr);
    time_t dueTime = mktime(&dueTm);
    double diffSeconds = difftime(dueTime, now);
    return diffSeconds > 0 && diffSeconds <= hoursAhead * 3600;
}

struct Task {
    int priority;
    string description;
    string timestamp;  // when created
    string dueDate;    // optional due date string
};

class MinHeap {
    Task tasks[MAX_TASKS];
    int size;

public:
    MinHeap() { size = 0; }

    void insertTask(int priority, string desc, string dueDate) {
        if (size >= MAX_TASKS) {
            cout << RED << "Task list full!" << RESET << endl;
            return;
        }

        Task newTask;
        newTask.priority = priority;
        newTask.description = desc;
        newTask.timestamp = getCurrentDateTime();
        newTask.dueDate = dueDate;

        int i = size++;
        tasks[i] = newTask;

        while (i > 0 && tasks[i].priority < tasks[(i - 1) / 2].priority) {
            swap(tasks[i], tasks[(i - 1) / 2]);
            i = (i - 1) / 2;
        }

        cout << GREEN << "Task added at " << newTask.timestamp << RESET << endl;
    }

    void removeTask() {
        if (size == 0) {
            cout << RED << "No tasks to remove!" << RESET << endl;
            return;
        }

        cout << GREEN << "✅ Completed Task: " << tasks[0].description
             << " (Added: " << tasks[0].timestamp << ")" << RESET << "\n";

        tasks[0] = tasks[--size];

        int i = 0;
        while (true) {
            int left = 2 * i + 1, right = 2 * i + 2, smallest = i;

            if (left < size && tasks[left].priority < tasks[smallest].priority)
                smallest = left;
            if (right < size && tasks[right].priority < tasks[smallest].priority)
                smallest = right;
            if (smallest == i) break;

            swap(tasks[i], tasks[smallest]);
            i = smallest;
        }
    }

    void peekTask() {
        if (size == 0) {
            cout << RED << "No tasks available!\n" << RESET;
        } else {
            cout << CYAN << "🔎 Next Task: " << tasks[0].description
                 << " (Priority " << tasks[0].priority
                 << ", Added: " << tasks[0].timestamp
                 << ", Due: " << (tasks[0].dueDate.empty() ? "None" : tasks[0].dueDate)
                 << ")" << RESET << "\n";
        }
    }

    void displayTasks() {
        if (size == 0) {
            cout << RED << "No tasks available!" << RESET << endl;
            return;
        }

        cout << "\n" << CYAN << "📝 Your To-Do List (Heap View):\n" << RESET;
        cout << CYAN << "---------------------------------------------------------------" << RESET << endl;
        for (int i = 0; i < size; i++) {
            string status = "";
            if (isOverdue(tasks[i].dueDate)) status = RED + " (OVERDUE)" + RESET;
            else if (isDueSoon(tasks[i].dueDate)) status = YELLOW + " (Due Soon)" + RESET;

            cout << "Priority: " << tasks[i].priority
                 << " | Added: " << tasks[i].timestamp
                 << " | Due: " << (tasks[i].dueDate.empty() ? "None" : tasks[i].dueDate)
                 << status
                 << " | Task: " << tasks[i].description << endl;
        }
        cout << CYAN << "---------------------------------------------------------------" << RESET << endl;
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        for (int i = 0; i < size; i++) {
            file << tasks[i].priority << "|" << tasks[i].timestamp << "|" << tasks[i].dueDate << "|" << tasks[i].description << "\n";
        }
        file.close();
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find("|");
            size_t pos2 = line.find("|", pos1 + 1);
            size_t pos3 = line.find("|", pos2 + 1);
            if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos) {
                int prio = stoi(line.substr(0, pos1));
                string ts = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string due = line.substr(pos2 + 1, pos3 - pos2 - 1);
                string desc = line.substr(pos3 + 1);

                Task t;
                t.priority = prio;
                t.timestamp = ts;
                t.dueDate = due;
                t.description = desc;

                if (size >= MAX_TASKS) continue;

                int i = size++;
                tasks[i] = t;
                while (i > 0 && tasks[i].priority < tasks[(i - 1) / 2].priority) {
                    swap(tasks[i], tasks[(i - 1) / 2]);
                    i = (i - 1) / 2;
                }
            }
        }
        file.close();
    }

    void exportToCSV(const string& filename) {
        ofstream file(filename);
        file << "Priority,Added,Due Date,Description\n";
        for (int i = 0; i < size; i++) {
            string desc = tasks[i].description;
            // Escape double quotes for CSV
            size_t pos = 0;
            while ((pos = desc.find("\"", pos)) != string::npos) {
                desc.replace(pos, 1, "\"\"");
                pos += 2;
            }
            file << tasks[i].priority << ",\"" << tasks[i].timestamp << "\",\"" << tasks[i].dueDate << "\",\"" << desc << "\"\n";
        }
        file.close();
        cout << GREEN << "Tasks exported to " << filename << RESET << endl;
    }

    void remindUrgentTasks() {
        bool hasUrgent = false;
        for (int i = 0; i < size; i++) {
            if (isOverdue(tasks[i].dueDate)) {
                cout << RED << "⚠️ Overdue Task: " << tasks[i].description << RESET << endl;
                hasUrgent = true;
            } else if (isDueSoon(tasks[i].dueDate)) {
                cout << YELLOW << "⏰ Due Soon: " << tasks[i].description << " due by " << tasks[i].dueDate << RESET << endl;
                hasUrgent = true;
            }
        }
        if (!hasUrgent) cout << GREEN << "No urgent tasks at the moment. Good job! 🎉" << RESET << endl;
    }
};

int main() {
    MinHeap toDoList;
    const string dataFilename = "tasks.txt";
    const string csvFilename = "tasks_export.csv";

    toDoList.loadFromFile(dataFilename);
    cout << CYAN << "\n--- Task Reminders on Startup ---\n" << RESET;
    toDoList.remindUrgentTasks();

    char choice;
    int priority;
    string description;
    string dueDate;

    do {
        cout << "\n" << CYAN << "🧠 Smart To-Do List\n" << RESET;
        cout << YELLOW
             << "a. Add Task\n"
             << "r. Remove Task\n"
             << "v. View Tasks\n"
             << "p. Peek Task\n"
             << "x. Export Tasks to CSV\n"
             << "e. Exit\n"
             << "Enter your choice: " << RESET;

        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 'a':
            case 'A':
                cout << YELLOW << "Enter Task Description: " << RESET;
                getline(cin, description);
                cout << YELLOW << "Enter Priority (1-5, 1 = Highest): " << RESET;
                cin >> priority;
                cin.ignore();

                if (priority < 1 || priority > 5) {
                    cout << RED << "❌ Invalid priority! Please enter 1 to 5.\n" << RESET;
                    break;
                }

                cout << YELLOW << "Enter Due Date and Time (YYYY-MM-DD HH:MM) or leave empty: " << RESET;
                getline(cin, dueDate);
                if (!dueDate.empty()) {
                    tm dummyTm = {};
                    if (!parseDateTime(dueDate, dummyTm)) {
                        cout << RED << "❌ Invalid date format! Use YYYY-MM-DD HH:MM\n" << RESET;
                        break;
                    }
                }

                toDoList.insertTask(priority, description, dueDate);
                break;

            case 'r':
            case 'R':
                toDoList.removeTask();
                break;

            case 'v':
            case 'V':
                toDoList.displayTasks();
                break;

            case 'p':
            case 'P':
                toDoList.peekTask();
                break;

            case 'x':
            case 'X':
                toDoList.exportToCSV(csvFilename);
                break;

            case 'e':
            case 'E':
                toDoList.saveToFile(dataFilename);
                cout << GREEN << "📁 Tasks saved. Goodbye!\n" << RESET;
                break;

            default:
                cout << RED << "❌ Invalid option. Try again.\n" << RESET;
        }
    } while (choice != 'e' && choice != 'E');

    return 0;
}

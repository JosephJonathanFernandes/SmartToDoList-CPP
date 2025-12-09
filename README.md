# Smart To-Do List (C++)

A command-line To-Do List manager built in C++ using a Min Heap to prioritize tasks efficiently.  
Includes due date tracking and colorful terminal output to highlight overdue and upcoming tasks.

---

## 🚀 Features

- 📌 **Add Tasks** with a priority level (1 = Highest)
- 📅 **Optional Due Date** with overdue/due soon highlighting
- 📊 **Min Heap** ensures top priority tasks always surface first
- 🎨 **Colored Output** using ANSI escape codes
- 🧹 Remove completed tasks easily
- 💾 Keeps tasks in-memory during session

---

This repository contains a compact, maintainable C++ to-do application implemented as a learning project.

Goals and improvements applied:
- Project layout with `CMake` and separated `include/` and `src/` directories.
- Safer data structures (use of `std::vector` and heap algorithms) for maintainability.
- Unit tests with `Catch2` and a CI workflow to build and run tests.
- Basic repository policies and documentation for contributors and security.

Quick start (Windows / PowerShell):

```powershell
mkdir build; cd build
cmake ..
cmake --build . --config Release
./smarttodo_app
```

Files of interest:
- `include/todo.h` — public API
- `src/todo.cpp` — implementation
- `src/main.cpp` — CLI entry point
- `tests/` — unit tests

This repository is marked as a learning project. See `LEARNING.md` for details.






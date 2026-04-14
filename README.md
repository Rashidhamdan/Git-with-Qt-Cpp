# Git GUI with Qt/C++

[Main Window](docs/screenshots/main_window.png)

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square)
![Qt](https://img.shields.io/badge/Qt-6-41CD52?style=flat-square)
![CMake](https://img.shields.io/badge/CMake-Build-064F8C?style=flat-square)
![Status](https://img.shields.io/badge/Status-Active%20Development-D4A017?style=flat-square)

---

## 🚀 Overview

**GIT_GUI** is a desktop Git client built with **C++** and **Qt Widgets** that provides a graphical interface for common Git workflows.

The project combines a modern, modular GUI architecture with practical Git integration through `QProcess`. It was built as a real-world portfolio project to demonstrate **system-level C++ development**, **Qt desktop UI engineering**, **modular software design**, and **repository-aware workflows** such as status inspection, staging, commits, diff viewing, branch handling, and upstream-based push/pull.

In addition to the graphical interface, the application also includes a **hybrid command console**: supported commands are routed through the application's own logic, while unsupported commands can fall back to Git directly.

---

## ✨ Features

### ✅ Implemented

- Open existing Git repositories
- Validate repository paths
- Repository workspace with modular widget structure
- File Explorer / Repository tree view
- Git Status UI
  - Staged files
  - Modified files
  - Untracked files
- Stage selected files
- Stage all changes
- Commit creation from the GUI
- Diff Viewer
- Log Viewer
- Branch management
  - Create local branches
  - Switch local branches
  - Checkout remote branches as local tracking branches
- Dynamic upstream-aware Push
- Dynamic upstream-aware Pull
- Toolbar and menu system
- Split-based desktop layout
- Status bar with repository, branch, remotes, and upstream information
- Integrated hybrid command console
- Global docked console with focus support
- Modular UI architecture

### 🟡 In Progress

- Repository initialization workflow polish
- Extended stash workflow
- Richer command routing and CLI feedback
- Additional branch management refinements
- UI consistency improvements
- Better workflow feedback and error handling polish

### 🔴 Planned

- Merge support
- Rebase support
- Tag management
- Conflict handling improvements
- More advanced Git operations
- Better command help / discovery
- Additional tests
- Packaging and release setup
- Commit graph visualization

---

## 🖼️ Screenshots

### Main Window

```markdown
![Main Window](https://github.com/Rashidhamdan/Git-with-Qt-Cpp/docs/screenshots/main_window.png)
```

### Repository Explorer

```markdown
![Repository Explorer](https://github.com/Rashidhamdan/Git-with-Qt-Cpp/docs/screenshots/repo_explorer.png)
```

### Status View

```markdown
![Status View](https://github.com/Rashidhamdan/Git-with-Qt-Cpp/docs/screenshots/status_view.png)
```

### Diff Viewer

```markdown
![Diff Viewer](https://github.com/Rashidhamdan/Git-with-Qt-Cpp/docs/screenshots/diff_viewer.png)
```

### Log Viewer

```markdown
![Log Viewer](https://github.com/Rashidhamdan/Git-with-Qt-Cpp/docs/screenshots/log_viewer.png)
```


### Branch Management

```markdown
![Branch Management](https://github.com/Rashidhamdan/Git-with-Qt-Cpp/docs/screenshots/branch_manager.png)
```


### Command Console

```markdown
![Command Console](https://github.com/Rashidhamdan/Git-with-Qt-Cpp/docs/screenshots/command_console.png)
```

---

## 🏗️ Project Structure
```bash
src/
├── gui/         # Main window, dialogs, widgets, console integration
├── commands/    # Git command wrappers (add, commit, status, diff, log, push, pull, ...)
├── core/        # Repository helpers, branch/upstream/remotes metadata
├── cli/         # Command parser and command router
├── utils/       # Logging, hashing, file helpers, compression, misc utilities
```

---

## 🧠 Goals

- Build a scalable desktop Git client with Qt Widgets
- Apply clean modular architecture in a real GUI application
- Practice Git process integration through ```QProcess```
- Separate GUI, command layer, repository logic, CLI routing, and utilities cleanly
- Combine GUI workflows with a command-driven interaction model
- Serve as a strong portfolio project for C++ / Qt desktop development

---

## ⚙️ Tech Stack

- C++17
- Qt 6 (Widgets)
- CMake
- Git CLI via ```QProcess```

---

## 📦 Build Requirements

Before building the project, make sure you have:

- A C++17-compatible compiler
- CMake 3.16+
- Qt 5 or Qt 6 with Widgets support
- Git installed and available in ```PATH```

```bash
git clone https://github.com/yourusername/GIT_GUI.git
cd GIT_GUI
mkdir build && cd build
cmake ..
make
./GIT-GUI
```
After building, run the generated executable for your platform.

### 📦 Prebuilt binaries for Linux and Windows are available from the repository’s GitHub Actions page.

---

## ▶️ Usage

Basic workflow:

1. Open a Git repository
2. Browse repository files
3. Inspect staged, modified, and untracked files
4. Stage selected files or stage all changes
5. Create a commit
6. Review diffs and commit history
7. Push or pull using the configured upstream
8. Manage branches from the GUI
9. Use the integrated console for supported commands and Git passthrough workflows

---

## 💻 Hybrid Command Console

The integrated console follows a hybrid model:

- Supported commands are executed through the application's internal logic
- Unsupported commands can fall back to Git directly
- Repository state changes triggered from the console can be reflected back into the GUI

This makes the application usable even while some GUI workflows are still being expanded.

---

## 📌 Status

🚧 Active development

This project is actively evolving and new functionality is being added continuously.

Current focus areas include:

- workflow completeness
- branch and stash handling improvements
- command integration polish
- advanced Git feature expansion
- UI refinement and consistency

---

## 📸 Screenshots Setup

Place screenshots in:
docs/screenshots/

---

## 💡 Why This Project Matters

This project demonstrates:

- Desktop application development with C++ and Qt
- Practical Git integration in a GUI application
- Modular architecture with clear separation of concerns
- Repository-aware workflows including status, commit, branch, and upstream handling
- A hybrid interaction model that combines GUI usability with command flexibility

---

## 👨‍💻 Author

Rashid Hamdan  

- GitHub: https://github.com/Rashidhamdan  
- Email: rashid.hamdan@outlook.de  
- LinkedIn: https://www.linkedin.com/in/rashid-hamdan-0307ab267

---

## 📄 License

This project is licensed under the MIT License.

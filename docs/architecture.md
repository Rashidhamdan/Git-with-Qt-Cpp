## Architecture Overview

Git-GUI is a modular desktop application built with **C++**, **Qt Widgets**, and **CMake**.

The project is organized around a clear separation between:

- graphical user interface
- Git command execution
- repository metadata access
- command-line routing inside the embedded console
- shared utility helpers

The application uses Git through **QProcess** and combines a GUI-first workflow with a hybrid command console. Supported workflows are handled through internal application logic, while unsupported console commands can fall back to Git directly.

## Architectural Goals

The main goals of the architecture are:

- keep the GUI modular and easy to extend
- separate Git operations from widget code
- make repository state visible across the application
- support both GUI interaction and command-driven workflows
- allow gradual expansion without blocking usability

This makes the project suitable both as a usable desktop tool and as a portfolio project showing practical software structure.

## High-Level Structure

The project is divided into these major modules:

- `src/gui`
- `src/commands`
- `src/core`
- `src/cli`
- `src/utils`

Each module has a distinct responsibility.

## Module Responsibilities

### GUI Layer

**Location:** `src/gui`

The GUI layer contains the desktop interface and the main user interaction flow.

Important classes include:

- `MainWindow`
- `RepoWidget`
- `StatusWidget`
- `RepoExplorerWidget`
- `DiffViewerWidget`
- `LogViewerWidget`
- `CommandConsoleWidget`
- `CommitDialog`
- `BranchManagerDialog`
- `InitRepoDialog`
- `StashDialog`

#### MainWindow

`MainWindow` is the top-level application shell.

**Responsibilities:**

- application menus
- toolbar actions
- status bar information
- repository open and close workflow
- global command console dock
- integration of dialogs and repository workspace
- coordination of UI updates after repository changes

#### RepoWidget

`RepoWidget` is the central repository workspace.

**Responsibilities:**

- host the repository-specific UI
- combine explorer, status, log, diff, and console-related workflows
- trigger refresh operations
- execute repository-related actions through command classes
- emit state-change notifications upward to `MainWindow`

#### StatusWidget

`StatusWidget` is the main workflow widget for daily Git actions.

**Responsibilities:**

- display staged, modified, and untracked files
- allow staging selected files
- trigger commit, push, pull, and refresh actions
- reflect repository state inside the UI

#### RepoExplorerWidget

`RepoExplorerWidget` presents the repository file tree.

**Responsibilities:**

- browse repository files
- select files for diff display
- notify the workspace when a file changes selection
- support navigation of the working tree

#### DiffViewerWidget

`DiffViewerWidget` renders file diffs.

**Responsibilities:**

- display selected file diff
- format diff text for readability
- provide refresh support
- keep repository and file context visible

#### LogViewerWidget

`LogViewerWidget` displays repository history.

**Responsibilities:**

- show commit list
- show commit details
- react to history refreshes
- provide commit metadata in a readable format

#### CommandConsoleWidget

`CommandConsoleWidget` provides a hybrid console inside the GUI.

**Responsibilities:**

- accept textual commands
- parse and route commands
- execute supported internal commands
- fall back to Git passthrough where appropriate
- emit repository state updates so the GUI stays synchronized

### Command Layer

**Location:** `src/commands`

The command layer wraps Git operations into focused C++ classes.

Implemented command classes include:

- `GitCommandAdd`
- `GitCommandCommit`
- `GitCommandStatus`
- `GitCommandLog`
- `GitCommandDiff`
- `GitCommandPush`
- `GitCommandPull`

Additional command classes exist for broader future coverage, such as:

- `GitCommandBranch`
- `GitCommandCheckout`
- `GitCommandInit`

#### Purpose of the Command Layer

The command layer prevents Git process execution from being scattered across widget code.

Each command class is responsible for:

- validating input
- executing the Git process
- collecting `stdout` and `stderr`
- converting the result into a structured response object

This keeps the UI cleaner and makes the backend logic easier to extend.

### Core Layer

**Location:** `src/core`

The core layer contains repository-related helpers and Git-oriented domain classes.

Important classes include:

- `GitRepository`
- `GitRepositoryInfo`
- `GitObject`
- `GitBlob`
- `GitTree`
- `GitCommit`
- `GitTag`
- `GitIndex`

#### GitRepositoryInfo

`GitRepositoryInfo` is currently one of the most important classes in the core layer.

**Responsibilities:**

- detect repository root
- detect current branch
- list remotes
- detect upstream configuration
- validate whether a directory is a Git repository

This class is used by `MainWindow`, `RepoWidget`, and branch-related workflows.

#### Domain Objects

The classes `GitObject`, `GitBlob`, `GitTree`, `GitCommit`, `GitTag`, and `GitIndex` represent the start of a deeper Git domain model.

At the current stage, they serve more as architectural groundwork than as the central runtime engine of the application.

### CLI Layer

**Location:** `src/cli`

The CLI layer supports the embedded console through structured parsing and routing.

Key classes:

- `CommandParser`
- `CommandRouter`

#### CommandParser

**Responsibilities:**

- receive raw console input
- normalize and tokenize the command string
- identify the command category and arguments

#### CommandRouter

**Responsibilities:**

- decide whether a command is internally supported
- dispatch to internal workflows when available
- use Git passthrough when appropriate
- return clear feedback to the console widget

This design allows the application to remain usable even when some features are not yet exposed through the GUI.

### Utils Layer

**Location:** `src/utils`

The utils layer contains helper functions used across the project.

Important helpers include:

- `Logger`
- `FileUtils`
- `HashUtils`
- `CompressionUtils`

These helpers support shared technical concerns without placing them directly inside GUI or command classes.

## Data Flow

A typical workflow follows this pattern:

`GUI action`
→ `repository widget or main window handler`
→ `command object`
→ `Git CLI via QProcess`
→ `structured result`
→ `widget refresh`
→ `updated repository state shown in the UI`

### Example: Commit

`User triggers commit`
→ `MainWindow opens CommitDialog`
→ `commit message is collected`
→ `RepoWidget calls GitCommandCommit`
→ `Git process runs`
→ `success or error result is returned`
→ `UI refresh follows`

### Example: Push / Pull

`User triggers push or pull` 
→ `RepoWidget resolves current branch and upstream through GitRepositoryInfo` 
→ `GitCommandPush or GitCommandPull is configured with remote and branch` 
→ `Git process runs` 
→ `repository state refreshes afterward`

### Example: Console Command

`User enters a command in CommandConsoleWidget` 
→ `CommandParser parses it` 
→ `CommandRouter selects internal command handling or passthrough` 
→ `result is shown in the console` 
→ `repository state update is emitted when needed` 
→ `GUI refresh follows`

## Repository State Model

The current application state is mainly coordinated through:

- current repository path
- current branch
- current remotes
- current upstream
- staged file presence
- repository refresh signals

This state is not yet consolidated into a single full repository view model. Instead, it is shared between `MainWindow`, `RepoWidget`, and specific widgets through method calls and signals.

This is sufficient for the current stage of the project and keeps the workflow practical.

## Branch and Remote Handling

Branch and remote behavior is no longer hardcoded to a fixed target such as `origin/main`.

The current workflow uses:

- the detected current local branch
- the repository’s configured remotes
- the upstream of the active branch

This allows push and pull to follow the actual repository configuration and makes the application more realistic for real-world repositories.

## Hybrid Console Strategy

One of the defining design choices of the project is the hybrid console strategy.

The console is not just a debug terminal. It is part of the actual product design.

The strategy is:

- use internal logic where functionality already exists in the application
- allow direct Git passthrough where functionality is not yet fully implemented
- keep the GUI and repository state synchronized after command execution

This has two advantages:

- the tool remains useful while features are still evolving
- the architecture demonstrates a practical bridge between graphical workflows and command-driven workflows

## Signal and Update Model

Qt signals and slots are used to keep the application synchronized.

Typical update triggers include:

- repository opened
- repository closed
- commit created
- branch changed
- console command affecting repository state
- push or pull finished
- refresh triggered from status, diff, or log view

When these events occur, the application refreshes repository metadata and updates the visible UI state.

## Current Strengths

The current architecture already demonstrates several strong design choices:

- modular separation of concerns
- repository workspace separated from application shell
- command wrappers separated from widgets
- branch and upstream awareness
- hybrid GUI and CLI interaction model
- expandable structure for future features

## Current Limitations

The architecture is functional and clean enough for real use, but some areas are still evolving.

Examples:

- some dialogs are still less mature than the core workflow
- some advanced Git operations are not fully modeled yet
- repository state is not yet centralized in a richer model object
- some domain-layer classes are currently more foundational than fully active
- theme and design consistency are still being refined

These limitations are acceptable for the current portfolio stage and do not prevent the project from being usable.

## Why This Architecture Works for the Project

This architecture is well suited to the current goals of Git-GUI because it balances:

- usability
- modularity
- incremental expansion
- practical Git integration

It is structured enough to show professional software design, while still flexible enough to support ongoing development.

## Future Direction

Likely future architectural improvements include:

- richer repository state modeling
- stronger branch and remote workflow support
- more internal CLI command coverage
- deeper integration between history, diff, and branch actions
- broader test coverage
- stronger design-system consistency across the UI

## Summary

Git-GUI uses a modular Qt/C++ architecture with a clear split between GUI, command execution, core repository information, CLI routing, and shared utilities.

The result is a practical Git desktop application that already supports real workflows while leaving room for structured expansion.

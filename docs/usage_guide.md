## Usage Guide

### Overview

Git-GUI is a desktop Git client built with C++ and Qt Widgets.

It provides a graphical workflow for common Git operations and includes an integrated hybrid command console for supported internal commands and Git passthrough.

### Open a Repository

To open a repository:

- Use **File -> Open Repository**
- Or click the **Open** action in the toolbar

Select a valid Git repository folder.
If the folder is not a Git repository, the application shows a warning.

After opening a repository, the application updates:

- repository explorer
- status view
- log view
- diff view
- branch and upstream information
- command console context

### Main Areas

#### Toolbar

The toolbar provides quick access to:

- open repository
- close repository
- status
- add
- commit
- branches
- log
- diff
- push
- pull
- focus console

#### Repository Workspace

The main workspace contains:

- repository explorer
- status tab
- log tab
- diff tab

#### Status Bar

The status bar shows:

- repository
- current branch
- remotes
- upstream

#### Global Command Console

The command console is available as a dock widget.

It supports:

- internal application commands
- Git passthrough for unsupported commands

### Status Workflow

The status view shows:

- staged files
- modified files
- untracked files

Available actions:

- **Add Selected**
- **Commit**
- **Push**
- **Pull**
- **Refresh**

#### Add Selected

Select files in the status lists and click **Add Selected**.

#### Stage All Changes

Use the main **Add** action from the menu or toolbar to stage all changes.

#### Commit

To create a commit:

1. Open the commit dialog
2. Enter a summary
3. Optionally enter a description
4. Confirm the dialog

The application validates the message, runs the commit, refreshes the repository state, and shows the commit hash when available.

### Diff Viewer

The diff viewer shows the diff for the selected file.

It supports:

- repository and file context
- formatted diff output
- refresh
- placeholder messages when no diff is available

### Log Viewer

The log viewer shows repository history.

It includes:

- a commit list
- a details panel for the selected commit

Displayed details may include:

- hash
- author
- date
- refs
- message

### Branch Management

Open the branch manager from:

- **Git -> Branches**
- or the **Branches** toolbar action

Current branch workflows include:

- viewing branches
- switching local branches
- creating branches
- checking out remote branches as local tracking branches

After a branch change, the UI refreshes automatically.

### Push and Pull

Push and pull use the configured upstream of the current branch.

They are only available when:

- a repository is open
- an upstream is configured

If no upstream is configured, the application shows a warning.

### Command Console

The integrated console supports a hybrid model:

- supported commands use internal application logic
- unsupported commands can fall back to Git directly

Console-triggered repository changes update the GUI automatically.

### Focus Console

Use the **Focus Console** action from the toolbar or view menu to:

- open the console if it is hidden
- bring it to the foreground
- focus it for input

### Current Scope

#### Implemented and usable

- open repository
- status view
- stage selected files
- stage all changes
- commit
- diff viewer
- log viewer
- branch workflows
- upstream-aware push and pull
- integrated console

#### Still expandable

- init workflow
- stash workflow
- more advanced Git operations
- richer console command coverage

# MiniGit: A Custom Version Control System

## Description

MiniGit is a lightweight, Git-inspired version control system built from scratch in C++. It allows basic Git-like operations such as:

- Initializing a repository
- Adding and committing files
- Creating branches
- Switching between branches or commits
- Merging branches with conflict handling
- Viewing commit history
- (Optional) Diffing between commits

All operations are done via a Command-Line Interface (CLI), simulating a local Git workflow.

## Sample Usage

$ ./minigit
welcom to MiniGit!

Enter command: init
Initialized empty MiniGit repository!

Enter command: add notes.txt
Staged notes.txt -> 123456789

Enter command: commit -m "Initial commit"
Committed with ID: 1750382365

Enter command: branch feature
Created new branch: feature

Enter command: checkout feature
Switched to branch: feature

## Edit notes.txt...

Enter command: add notes.txt
Enter command: commit -m "Updated notes on feature branch"
Committed with ID: 1750389987

Enter command: checkout main
Switched to branch: main

## Edit notes.txt differently...

Enter command: add notes.txt
Enter command: commit -m "Updated notes on main"
Committed with ID: 1750391040

Enter command: merge feature
CONFLICT: both modified notes.txt
Merge done. Resolve any conflicts, then commit.

## Fix the file manually...

Enter command: add notes.txt
Enter command: commit -m "Resolved merge conflict"
Committed with ID: 1750393617

## How to Build and Run

g++ main.cpp minigit.cpp -o minigit -std=c++17
./minigit
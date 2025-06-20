# MiniGit: A Custom Version Control System

## Description

MiniGit is a lightweight, C++-based version control system that mimics the core features of Git. Built from scratch without relying on external libraries, this CLI tool enables file tracking, commits, branches, merges, and conflict resolution. It's designed to help students deeply understand the data structures and algorithms that power real-world systems like Git.

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
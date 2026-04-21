# CodeJudge


## A project for Software Project Lab 1


A Lightweight C Program Testing and Static Analysis Tool


## Overview

CodeJudge is a terminal-based application written in C that simulates a mini online judge system. It allows users to compile, test, and analyze C programs locally with features like automated test execution, output comparison, and static code analysis.

This project combines systems programming, file handling, and basic security analysis into one unified tool.


## Features

### Test Case Execution

- Run multiple test cases per program
- Input methods: keyboard, file, or automatic
- Output capture and comparison with expected output
- Pass/Fail detection

### Compilation System

- Compiles C programs using GCC
- Displays compilation errors clearly
- Prevents execution if compilation fails

### Result Dashboard

- Total test cases with passed/failed count and pass percentage
- Detailed per-test results

### Code Metrics Analysis

- Basic source code metrics
- Cyclomatic Complexity
- Cognitive Complexity
- Nesting depth analysis

### Buffer Overflow Risk Detection

- Tracks character buffers
- Detects unsafe functions: `gets`, `strcpy`, `strcat`, `sprintf`, etc.
- Identifies tainted data from user input
- Generates a risk score and risk level (LOW / MEDIUM / HIGH)

### Multi-Session Support

- Load multiple source files
- Switch between files
- Maintain separate test histories

### Timeout Protection

- Prevents infinite loops
- Automatically stops long-running programs


## Project Structure

```
CodeJudge/
├── src/
│   ├── codejudge.c
│   ├── Testing.h
│   ├── Testing.c
│   ├── cognitive.c
│   ├── cyclomatic.c
│   ├── mathlib.c
│   ├── mathlib.h
│   ├── metrics.c
│   ├── metrics.h
│   └── output_com.c
├── t/
│   ├── buff.c
│   └── dij.c
└── README.md
```


## Installation

### Requirements

- GCC Compiler
- Linux, macOS, or WSL (recommended)

### Compile

```bash
gcc src/codejudge.c src/cognitive.c  src/cyclomatic.c src/metrics.c src/mathlib.c src/output_com.c src/Buffer_overflow1.c -o codejudge
```

### Run

```bash
./codejudge
```


## Usage

1. Run the program
2. Enter the path to your C source file
3. Use the menu to run test cases, view results, analyze code, or check buffer overflow risks
4. Provide input and expected output
5. View results instantly

### Sample Menu

```
=================== MENU ===================
1. Run Test Case
2. View Test Results
3. Analyze Code Metrics
4. Advanced Metrics
5. Buffer Overflow Risk Analysis
6. Add New File
7. Switch File
8. Exit
============================================
```


## Core Concepts Used

- Dynamic Memory Allocation (`malloc`, `realloc`, `free`)
- File Handling (`fopen`, `fgets`, `fprintf`)
- Process Execution (`system`)
- Static Code Analysis
- Basic Taint Analysis
- Struct-based Design (C-style OOP)


## Supervisor

**Md. Saeed Siddik**


Assistant Professor


IIT,University of Dhaka



## Author

**Fahmida Munni**


Software Engineering


Institute of Information Technology, University of Dhaka

Overview
CodeJudge is a command-line tool that automates the testing and quality analysis of C programs. It compiles code, executes test cases, validates output, and provides comprehensive code quality metrics.
Key Capabilities:

Automated compilation and execution
Multiple test case management
Runtime error detection
Code quality metrics analysis
Buffer overflow risk detection


Features
1. Automated Testing System

Run programs against custom test cases
Input from keyboard or file
Expected output comparison
Timeout protection (10 seconds)

2. Error Detection

Compilation Errors: Captures GCC output
Runtime Errors: Detects segmentation faults, crashes
Timeout Detection: Catches infinite loops

3. Code Metrics Analysis

Lines of Code (LOC)
Logical Lines of Code (LLOC)
Halstead Complexity Metrics
Comment percentage

4. Advanced Complexity Metrics

Cyclomatic Complexity: Measures branching complexity
Cognitive Complexity: Measures understandability

5. Buffer Overflow Detection

Tracks buffer declarations
Identifies unsafe functions
Taint analysis for user input
Per-buffer risk assessment

6. Output Visualization

Edit Distance calculation
Longest Common Subsequence (LCS)
Visual difference highlighting


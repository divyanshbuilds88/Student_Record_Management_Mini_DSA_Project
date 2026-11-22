# Student Record Management System

A beginner-friendly **Data Structures and Algorithms (DSA) mini project** written in C++.

The project shows how basic DSA concepts such as **arrays, structures, searching, sorting, insertion, deletion and traversal** are used to solve a real problem: managing student records. It comes in two versions that share the same data file:

| Version | Files | Description |
|---|---|---|
| Console version | `main.cpp` | Menu-driven program that runs in the terminal |
| Web version | `server.cpp` + `frontend/` | C++ backend with an HTML, CSS and JavaScript frontend that runs in the browser |

Student records are saved in `students.txt`, so they are still there after you close and restart the program.

---

## Table of Contents

1. [About the Project](#about-the-project)
2. [Features](#features)
3. [DSA Concepts Explained](#dsa-concepts-explained)
4. [Project Structure](#project-structure)
5. [How the Web Version Works](#how-the-web-version-works)
6. [Technologies Used](#technologies-used)
7. [Setup on Windows](#setup-on-windows)
8. [How to Run](#how-to-run)
9. [Input Validation](#input-validation)
10. [Data File Format](#data-file-format)
11. [Sample Output](#sample-output)
12. [Troubleshooting](#troubleshooting)
13. [Future Improvements](#future-improvements)

---

## About the Project

This is a DSA mini project. Its goal is to practise the core ideas of data structures and algorithms on something practical, instead of only reading about them in theory.

All records live in a simple **array of structures**. Every feature of the program is built from a basic DSA operation:

- Adding a student is **insertion**.
- Removing a student is **deletion**.
- Showing all students is **traversal**.
- Finding a student by roll number is **linear search**.
- Ordering students by marks is **bubble sort**.

The code is kept simple on purpose. It uses no databases, no external libraries and no advanced frameworks, so every line can be understood by a beginner.

---

## Features

1. **Add Student** - insert a new record (duplicate roll numbers are blocked)
2. **Display All Students** - show every record in a table
3. **Search Student** - find a student by roll number
4. **Delete Student** - remove a record
5. **Update Student** - change the name, course or marks of an existing student
6. **Sort Students by Marks** - highest to lowest
7. **Calculate Average Marks** - average of all students
8. **Display Topper** - student with the highest marks (ties are shown together)
9. **Exit** - close the program (console version)

Extra features:

- Permanent storage using file handling
- Input validation on every field
- A web interface with summary cards (total students, average marks, topper)

---

## DSA Concepts Explained

This section explains each concept, where it is used in the project, and how fast it is.

In the complexity notes, **n** means the number of students stored.

### 1. Array

**What it is:** An array stores many items of the same type in one continuous block of memory. Each item has a position number called an index, starting from 0.

**Where it is used:** All student records are stored in one array.

```cpp
const int MAX_STUDENTS = 100;
Student students[MAX_STUDENTS];   // the array
int studentCount = 0;             // how many slots are currently used
```

Imagine 100 numbered boxes in a row. `studentCount` tells us how many boxes are filled. The rest are empty and ready for new records.

**Why use it:** Reading any element by index (for example `students[3]`) is very fast, because the computer jumps straight to that position. This is **O(1)** time.

**Limitation:** The size is fixed. This project can hold at most 100 students.

### 2. Structure (struct)

**What it is:** A structure groups different pieces of data that belong together into one custom data type.

**Where it is used:** One `Student` structure holds all details of a single student.

```cpp
struct Student {
    int rollNo;
    string name;
    string course;
    float marks;
};
```

An array of structures (`Student students[100]`) works like a table. Each row is one student, and each column is one field.

| Index | rollNo | name | course | marks |
|---|---|---|---|---|
| 0 | 2546101 | Aarav Sharma | B.Tech CSE | 88.5 |
| 1 | 2546102 | Priya Singh | B.Tech CSE | 92 |
| 2 | 2546103 | Rohan Verma | B.Tech IT | 76.25 |

### 3. Insertion

**What it is:** Adding a new element to a data structure.

**Where it is used:** `addStudent()` places the new student in the next empty slot, at the end of the array.

```cpp
students[studentCount] = s;   // put the new student in the first empty slot
studentCount++;               // one more student is now stored
```

**Time complexity:** Inserting at the end is **O(1)**. Before inserting, the program checks for a duplicate roll number with a linear search, so the full add operation is **O(n)**.

### 4. Deletion

**What it is:** Removing an element from a data structure.

**Where it is used:** `deleteStudent()` removes a student. Arrays cannot have a gap in the middle, so every element after the deleted one is **shifted one position to the left**.

```cpp
for (int i = index; i < studentCount - 1; i++) {
    students[i] = students[i + 1];   // move each element one step left
}
studentCount--;                      // reduce the count
```

Example: deleting the student at index 1 from `[A, B, C, D]`:

```
Before:  A  B  C  D
Shift:   A  C  D  D   (C and D move left)
Count-1: A  C  D
```

**Time complexity:** **O(n)**, because in the worst case many elements have to be shifted.

### 5. Traversal

**What it is:** Visiting every element of a data structure exactly once.

**Where it is used:** `displayAll()` walks through the array from the first student to the last and prints each one. The average and topper calculations also traverse the array.

```cpp
for (int i = 0; i < studentCount; i++) {
    printStudent(students[i]);
}
```

**Time complexity:** **O(n)**.

### 6. Linear Search

**What it is:** A searching method that checks elements one by one from the start until the target is found or the list ends.

**Where it is used:** `findStudentIndex()` searches for a roll number. It returns the index if found, or `-1` if not. It is also used to block duplicate roll numbers, and to find the student to update or delete.

```cpp
int findStudentIndex(int rollNo) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNo == rollNo) {
            return i;       // found: return its position
        }
    }
    return -1;              // not found
}
```

**Time complexity:**

- Best case: **O(1)**, when the student is the first one
- Worst case: **O(n)**, when the student is last or missing

**Why linear search here:** The roll numbers are not kept in sorted order, because the array is sorted by marks. Linear search works on any unsorted list, which makes it the right choice. (Binary search is faster, but it needs data sorted by roll number.)

### 7. Bubble Sort

**What it is:** A sorting algorithm that repeatedly compares two neighbouring elements and swaps them if they are in the wrong order. After each full pass, the next largest value "bubbles" to its correct position.

**Where it is used:** `sortByMarks()` arranges students from the **highest marks to the lowest**.

```cpp
for (int i = 0; i < studentCount - 1; i++) {
    bool swapped = false;
    for (int j = 0; j < studentCount - i - 1; j++) {
        if (students[j].marks < students[j + 1].marks) {
            Student temp = students[j];        // swap the two students
            students[j] = students[j + 1];
            students[j + 1] = temp;
            swapped = true;
        }
    }
    if (!swapped) break;    // no swaps means already sorted: stop early
}
```

Example on marks `[70, 90, 80]`, sorting highest to lowest:

```
Pass 1:  70 90 80  ->  90 70 80  ->  90 80 70
Pass 2:  90 80 70  ->  no swaps, so it is sorted
```

**Time complexity:**

- Worst and average case: **O(n²)**
- Best case (already sorted): **O(n)**, thanks to the `swapped` check that stops early

Bubble sort is slower than advanced algorithms for big data. But it is simple to understand, and it is fast enough for a class-sized list of students.

### 8. File Handling (permanent storage)

**What it is:** Reading from and writing to files so that data survives after the program closes.

**Where it is used:**

- `loadFromFile()` runs once when the program starts and fills the array from `students.txt`.
- `saveToFile()` runs after every change (add, update, delete, sort) and writes the whole array back to `students.txt`.

### Complexity Summary

| Operation | Concept | Time complexity |
|---|---|---|
| Access one student by index | Array | O(1) |
| Add student | Insertion (+ duplicate check) | O(n) |
| Delete student | Deletion (shifting) | O(n) |
| Display all students | Traversal | O(n) |
| Search by roll number | Linear Search | O(n) |
| Sort by marks | Bubble Sort | O(n²) |
| Average marks | Traversal | O(n) |
| Find topper | Traversal | O(n) |

---

## Project Structure

```
Student_Record_Management/
|-- main.cpp            Console version (menu-driven program)
|-- server.cpp          C++ backend: web server and REST API
|-- students.txt        Saved student records
|-- frontend/
|   |-- index.html      Page structure
|   |-- style.css       Page styling
|   +-- script.js       Frontend logic (calls the backend)
|-- README.md           This file
+-- .gitignore          Keeps compiled .exe files out of Git
```

---

## How the Web Version Works

The web version has two parts that talk to each other over HTTP.

```
 Browser                          C++ Server                       File
 (HTML, CSS, JavaScript)          (server.cpp)                     (students.txt)
 +------------------+   JSON      +----------------------+  read/  +--------------+
 |  Form, table,    | ----------> |  Array, search,      | ------> |  saved       |
 |  buttons         | <---------- |  sort, insert, delete| <------ |  records     |
 +------------------+   response  +----------------------+  write  +--------------+
```

- The **frontend** is what you see. When you click a button, `script.js` sends a request to the server.
- The **backend** runs the DSA code, saves the file, and sends the result back as JSON.

### REST API

| Method | URL | What it does | DSA used |
|---|---|---|---|
| GET | `/api/students` | List all students | Traversal |
| GET | `/api/students/{roll}` | Find one student | Linear Search |
| POST | `/api/students` | Add a student | Insertion |
| PUT | `/api/students/{roll}` | Update a student | Linear Search |
| DELETE | `/api/students/{roll}` | Delete a student | Deletion |
| POST | `/api/sort` | Sort by marks | Bubble Sort |
| GET | `/api/stats` | Total, average and topper | Traversal |

The server uses only standard C++ and sockets. No library needs to be downloaded.

---

## Technologies Used

| Part | Technology |
|---|---|
| Language | C++ |
| Compiler | g++ (MinGW-w64) |
| Editor | Visual Studio Code |
| Frontend | HTML, CSS, JavaScript |
| Data storage | Plain text file (`students.txt`) |
| Project type | Console application and local web application |

---

## Setup on Windows

Do this once.

1. Install **Visual Studio Code** from https://code.visualstudio.com
2. In VS Code, open the Extensions tab (Ctrl + Shift + X), then search for and install the **C/C++** extension by Microsoft.
3. Install the **g++ compiler** with MSYS2:
   - Download and install MSYS2 from https://www.msys2.org
   - Open **MSYS2 UCRT64** and run:
     ```
     pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
     ```
   - Add `C:\msys64\ucrt64\bin` to your Windows **PATH**.
4. Open a new terminal and check the installation:
   ```
   g++ --version
   ```

---

## How to Run

Open the project folder in VS Code (**File > Open Folder**), then open a terminal with **Terminal > New Terminal**.

### Option 1: Console version

```
g++ main.cpp -o main.exe
.\main.exe
```

Type a number from 1 to 9 to use the menu.

### Option 2: Web version

```
g++ server.cpp -o server.exe -lws2_32
.\server.exe
```

Then open **http://localhost:8080** in your browser. Press **Ctrl + C** in the terminal to stop the server.

Notes:

- Always run the program from inside the project folder. It looks for `students.txt` and the `frontend` folder there.
- `-lws2_32` is needed only on Windows. On Linux or Mac, use `g++ server.cpp -o server`.
- Do not run the console version and the web server at the same time, because they share `students.txt`.

---

## Input Validation

| Field | Rule |
|---|---|
| Roll number | Exactly 7 digits (1000000 to 9999999), and must be unique |
| Name | Not empty, up to 50 characters, no `\|` character |
| Course | Not empty, up to 50 characters, no `\|` character |
| Marks | A number from 0 to 100 |
| Menu choice | A whole number from 1 to 9 |

In the web version, the browser checks the input first and the C++ backend checks it again. This keeps the data safe even if someone bypasses the page.

---

## Data File Format

Each student is stored on one line of `students.txt`, with the fields separated by `|`.

```
rollNo|name|course|marks
```

Example:

```
2546101|Aarav Sharma|B.Tech CSE|88.5
2546102|Priya Singh|B.Tech CSE|92
2546103|Rohan Verma|B.Tech IT|76.25
```

That is why the `|` character is not allowed inside a name or course. To start with an empty list, delete the contents of the file but keep the file itself.

---

## Sample Output

### Console menu

```
=====================================
   STUDENT RECORD MANAGEMENT SYSTEM
=====================================
  1. Add Student
  2. Display All Students
  3. Search Student
  4. Delete Student
  5. Update Student
  6. Sort Students by Marks
  7. Calculate Average Marks
  8. Display Topper
  9. Exit
=====================================
Enter Your Choice:
```

### Display all students

```
Roll No   Name                     Course              Marks
---------------------------------------------------------------
2546102   Priya Singh              B.Tech CSE          92.00
2546101   Aarav Sharma             B.Tech CSE          88.50
2546103   Rohan Verma              B.Tech IT           76.25

Total students: 3
```

### Web version

The page shows three summary cards (Total Students, Average Marks, Topper), an Add/Update form on the left, and a records table with Search, Show All and Sort buttons on the right. The topper is marked with a badge.

---

## Troubleshooting

**`g++ is not recognized`**
The compiler's folder is not in your PATH. Add `C:\msys64\ucrt64\bin` to the PATH, then close and reopen VS Code.

**`An Application Control policy has blocked this file`**
Windows is blocking the `.exe` you compiled, because it is unsigned. Try these in order:
1. Move the project out of the Downloads folder (for example to `C:\Projects`) and compile again.
2. On Windows 11, check **Windows Security > App & browser control > Smart App Control**.
3. On a college or work laptop, the policy is set by the administrator. Use WSL (Ubuntu on Windows) instead and compile with `g++ server.cpp -o server`.

**`Could not use port 8080`**
The server is already running in another terminal, or another program uses port 8080. Close the other one, or change `PORT` at the top of `server.cpp`.

**The page says "Cannot reach the server"**
`server.exe` is not running. Start it and refresh the page.

**Records do not show up**
Check that you started the program from inside the project folder, so it can find `students.txt`.

---

## Future Improvements

- Use **binary search** after sorting by roll number, for faster searching
- Replace bubble sort with a faster algorithm such as **merge sort** or **quick sort**
- Use a **linked list** or a **vector** so the number of students is not limited to 100
- Use a **hash table** for instant roll number lookup
- Add login for teachers and students
- Store more details such as subject-wise marks, grades and attendance

---

## Author

Your Name - DSA Mini Project

*Built to learn and practise Data Structures and Algorithms in C++.*

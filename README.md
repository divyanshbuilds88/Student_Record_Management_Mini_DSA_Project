# Student Record Management System

A beginner-friendly DSA mini project in C++ with two ways to use it:

1. **Console version** (`main.cpp`) - menu-based program in the terminal.
2. **Web version** (`server.cpp` + `frontend/`) - C++ backend and an HTML/CSS/JavaScript frontend.

Both versions use the same `students.txt` file, so records are saved permanently.
Do not run both at the same time.

## DSA Concepts Used

| Concept | Where it is used |
|---|---|
| Array | `students[]` stores all records (max 100) |
| Structure | `struct Student` holds roll no, name, course, marks |
| Insertion | add a record at the end of the array |
| Deletion | remove a record by shifting elements left |
| Traversal | visit every record to display it |
| Linear Search | `findStudentIndex()` searches by roll number |
| Bubble Sort | sort students by marks, highest to lowest |

## Project Structure

```
Student_Record_Management/
  main.cpp          - console version
  server.cpp        - C++ backend (web server + REST API)
  students.txt      - saved records  (rollNo|name|course|marks)
  frontend/
    index.html      - page structure
    style.css       - styling
    script.js       - frontend logic (calls the backend)
  README.md
  .gitignore
```

## How the Web Version Works

```
Browser (HTML/CSS/JS)  --HTTP/JSON-->  C++ server (server.cpp)  --read/write-->  students.txt
```

The browser shows the page. When you click a button, `script.js` sends a request to the C++ server,
the server runs the DSA code, saves the file, and sends the result back as JSON.

| Method | URL | Action |
|---|---|---|
| GET | `/api/students` | List all students |
| GET | `/api/students/{roll}` | Search one student |
| POST | `/api/students` | Add a student |
| PUT | `/api/students/{roll}` | Update a student |
| DELETE | `/api/students/{roll}` | Delete a student |
| POST | `/api/sort` | Sort by marks (bubble sort) |
| GET | `/api/stats` | Count, average, topper(s) |

No libraries or downloads are needed. The server uses only standard C++ and sockets.

## Setup (Windows, one time)

1. Install VS Code and the **C/C++** extension.
2. Install g++ through MSYS2 (https://www.msys2.org), then in the MSYS2 UCRT64 terminal run:
   `pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain`
3. Add `C:\msys64\ucrt64\bin` to your Windows PATH.
4. Check with `g++ --version` in a new terminal.

## Run the Web Version

Open the project folder in VS Code, open a terminal, and run:

```
g++ server.cpp -o server.exe -lws2_32
.\server.exe
```

Then open **http://localhost:8080** in your browser.
Press **Ctrl + C** in the terminal to stop the server.

Important: run `server.exe` from inside the project folder, because it looks for the `frontend` folder and `students.txt` there.

Note: `-lws2_32` is needed only on Windows. On Linux or Mac use `g++ server.cpp -o server`.

## Run the Console Version

```
g++ main.cpp -o main.exe
.\main.exe
```

## Input Validation

- Roll number: whole number from 1 to 999999, must be unique
- Name and course: not empty, up to 50 characters, no `|` character
- Marks: number from 0 to 100
- Validation happens in the browser and again in the C++ backend

## Features

Add, display, search, update, delete, sort by marks, average marks, and topper (ties are shown together).

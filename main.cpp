// ============================================================
//  Student Record Management System
//  A beginner-friendly DSA mini project in C++
//
//  DSA concepts used:
//    Array         -> students[] stores all records
//    Structure     -> struct Student holds one record
//    Insertion     -> addStudent()
//    Deletion      -> deleteStudent()
//    Traversal     -> displayAll()
//    Linear Search -> findStudentIndex()
//    Bubble Sort   -> sortByMarks()
//  Records are saved permanently in students.txt
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>
using namespace std;

const int MAX_STUDENTS = 100;                   // maximum records the array can hold
const string FILE_NAME = "students.txt";        // file used for permanent storage

// ---------- Structure: stores the details of ONE student ----------
struct Student {
    int rollNo;
    string name;
    string course;
    float marks;
};

// ---------- Array: stores ALL student records ----------
Student students[MAX_STUDENTS];
int studentCount = 0;                           // how many records are currently stored

// ============================================================
//  Input helper functions (basic input validation)
// ============================================================

// Reads a whole number between minValue and maxValue. Repeats until valid.
int readInt(const string &prompt, int minValue, int maxValue) {
    string line;
    while (true) {
        cout << prompt;
        getline(cin, line);
        stringstream ss(line);
        int value;
        char extra;
        // must be a number, with nothing extra after it
        if ((ss >> value) && !(ss >> extra) && value >= minValue && value <= maxValue) {
            return value;
        }
        cout << "  Invalid input! Enter a whole number between "
             << minValue << " and " << maxValue << ".\n";
    }
}

// Reads a decimal number between minValue and maxValue. Repeats until valid.
float readFloat(const string &prompt, float minValue, float maxValue) {
    string line;
    while (true) {
        cout << prompt;
        getline(cin, line);
        stringstream ss(line);
        float value;
        char extra;
        if ((ss >> value) && !(ss >> extra) && value >= minValue && value <= maxValue) {
            return value;
        }
        cout << "  Invalid input! Enter a number between "
             << minValue << " and " << maxValue << ".\n";
    }
}

// Reads a non-empty text. The '|' character is not allowed (used in the file).
string readText(const string &prompt) {
    string line;
    while (true) {
        cout << prompt;
        getline(cin, line);
        // remove spaces at the start and end
        size_t first = line.find_first_not_of(" \t");
        size_t last = line.find_last_not_of(" \t");
        if (first == string::npos) {
            cout << "  Input cannot be empty!\n";
            continue;
        }
        line = line.substr(first, last - first + 1);
        if (line.find('|') != string::npos) {
            cout << "  The '|' character is not allowed!\n";
            continue;
        }
        return line;
    }
}

// Waits for the user to press Enter
void pause() {
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

// ============================================================
//  File handling
// ============================================================

// Saves all records from the array into students.txt
// File format (one student per line):  rollNo|name|course|marks
void saveToFile() {
    ofstream file(FILE_NAME.c_str());
    if (!file) {
        cout << "  Error: could not save to file!\n";
        return;
    }
    for (int i = 0; i < studentCount; i++) {
        file << students[i].rollNo << "|" << students[i].name << "|"
             << students[i].course << "|" << students[i].marks << "\n";
    }
    file.close();
}

// Loads records from students.txt into the array when the program starts
void loadFromFile() {
    ifstream file(FILE_NAME.c_str());
    if (!file) {
        return;                                 // no file yet: start with empty list
    }
    string line;
    while (getline(file, line) && studentCount < MAX_STUDENTS) {
        if (line.empty()) continue;
        stringstream ss(line);
        string roll, name, course, marks;
        // split the line by the '|' character
        if (getline(ss, roll, '|') && getline(ss, name, '|') &&
            getline(ss, course, '|') && getline(ss, marks, '|')) {
            Student s;
            s.rollNo = atoi(roll.c_str());
            s.name = name;
            s.course = course;
            s.marks = (float)atof(marks.c_str());
            students[studentCount] = s;
            studentCount++;
        }
    }
    file.close();
}

// ============================================================
//  Core DSA operations
// ============================================================

// LINEAR SEARCH: checks every element one by one.
// Returns the index of the student, or -1 if not found.
int findStudentIndex(int rollNo) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNo == rollNo) {
            return i;
        }
    }
    return -1;
}

// Prints the table header
void printHeader() {
    cout << left << setw(10) << "Roll No" << setw(25) << "Name"
         << setw(20) << "Course" << setw(8) << "Marks" << "\n";
    cout << string(63, '-') << "\n";
}

// Prints one student as a table row
void printStudent(const Student &s) {
    cout << left << setw(10) << s.rollNo << setw(25) << s.name
         << setw(20) << s.course << fixed << setprecision(2) << s.marks << "\n";
}

// INSERTION: adds a new student at the end of the array
void addStudent() {
    cout << "\n--- Add Student ---\n";
    if (studentCount >= MAX_STUDENTS) {
        cout << "  List is full! Cannot add more students.\n";
        return;
    }

    Student s;
    s.rollNo = readInt("Enter Roll Number (1 - 999999): ", 1000000, 9999999);

    // Prevent duplicate roll numbers using linear search
    if (findStudentIndex(s.rollNo) != -1) {
        cout << "  Error: Roll number " << s.rollNo << " already exists!\n";
        return;
    }

    s.name = readText("Enter Name: ");
    s.course = readText("Enter Course: ");
    s.marks = readFloat("Enter Marks (0 - 100): ", 0, 100);

    students[studentCount] = s;                 // insert at the end
    studentCount++;
    saveToFile();
    cout << "  Student added successfully!\n";
}

// TRAVERSAL: visits every element of the array and prints it
void displayAll() {
    cout << "\n--- All Students ---\n";
    if (studentCount == 0) {
        cout << "  No records found.\n";
        return;
    }
    printHeader();
    for (int i = 0; i < studentCount; i++) {
        printStudent(students[i]);
    }
    cout << "\nTotal students: " << studentCount << "\n";
}

// Search a student by roll number (uses linear search)
void searchStudent() {
    cout << "\n--- Search Student ---\n";
    if (studentCount == 0) {
        cout << "  No records found.\n";
        return;
    }
    int roll = readInt("Enter Roll Number to search: ", 1000000, 9999999);
    int index = findStudentIndex(roll);
    if (index == -1) {
        cout << "  Student with roll number " << roll << " not found.\n";
    } else {
        cout << "  Student found!\n\n";
        printHeader();
        printStudent(students[index]);
    }
}

// DELETION: removes a student by shifting later elements one step left
void deleteStudent() {
    cout << "\n--- Delete Student ---\n";
    if (studentCount == 0) {
        cout << "  No records found.\n";
        return;
    }
    int roll = readInt("Enter Roll Number to delete: ", 1000000, 9999999);
    int index = findStudentIndex(roll);
    if (index == -1) {
        cout << "  Student with roll number " << roll << " not found.\n";
        return;
    }
    // shift every element after 'index' one position to the left
    for (int i = index; i < studentCount - 1; i++) {
        students[i] = students[i + 1];
    }
    studentCount--;
    saveToFile();
    cout << "  Student deleted successfully!\n";
}

// Updates the details of an existing student
void updateStudent() {
    cout << "\n--- Update Student ---\n";
    if (studentCount == 0) {
        cout << "  No records found.\n";
        return;
    }
    int roll = readInt("Enter Roll Number to update: ", 1000000, 9999999);
    int index = findStudentIndex(roll);
    if (index == -1) {
        cout << "  Student with roll number " << roll << " not found.\n";
        return;
    }
    cout << "  Current details:\n\n";
    printHeader();
    printStudent(students[index]);
    cout << "\nEnter new details (roll number cannot be changed):\n";
    students[index].name = readText("Enter New Name: ");
    students[index].course = readText("Enter New Course: ");
    students[index].marks = readFloat("Enter New Marks (0 - 100): ", 0, 100);
    saveToFile();
    cout << "  Student updated successfully!\n";
}

// BUBBLE SORT: repeatedly swaps neighbouring elements that are in the wrong
// order. Here we sort by marks from highest to lowest.
void sortByMarks() {
    cout << "\n--- Sort Students by Marks (Highest to Lowest) ---\n";
    if (studentCount == 0) {
        cout << "  No records found.\n";
        return;
    }
    for (int i = 0; i < studentCount - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (students[j].marks < students[j + 1].marks) {
                Student temp = students[j];     // swap the two students
                students[j] = students[j + 1];
                students[j + 1] = temp;
                swapped = true;
            }
        }
        if (!swapped) break;                    // already sorted: stop early
    }
    saveToFile();
    cout << "  Students sorted successfully!\n\n";
    printHeader();
    for (int i = 0; i < studentCount; i++) {
        printStudent(students[i]);
    }
}

// Calculates and shows the average marks of all students
void calculateAverage() {
    cout << "\n--- Average Marks ---\n";
    if (studentCount == 0) {
        cout << "  No records found.\n";
        return;
    }
    float total = 0;
    for (int i = 0; i < studentCount; i++) {
        total += students[i].marks;
    }
    cout << "  Total students : " << studentCount << "\n";
    cout << "  Average marks  : " << fixed << setprecision(2) << total / studentCount << "\n";
}

// Finds the student(s) with the highest marks
void displayTopper() {
    cout << "\n--- Topper ---\n";
    if (studentCount == 0) {
        cout << "  No records found.\n";
        return;
    }
    // step 1: find the highest marks
    float highest = students[0].marks;
    for (int i = 1; i < studentCount; i++) {
        if (students[i].marks > highest) {
            highest = students[i].marks;
        }
    }
    // step 2: print every student with those marks (handles ties)
    printHeader();
    for (int i = 0; i < studentCount; i++) {
        if (students[i].marks == highest) {
            printStudent(students[i]);
        }
    }
}

// Shows the main menu
void showMenu() {
    cout << "\n=====================================\n";
    cout << "   STUDENT RECORD MANAGEMENT SYSTEM\n";
    cout << "=====================================\n";
    cout << "  1. Add Student\n";
    cout << "  2. Display All Students\n";
    cout << "  3. Search Student\n";
    cout << "  4. Delete Student\n";
    cout << "  5. Update Student\n";
    cout << "  6. Sort Students by Marks\n";
    cout << "  7. Calculate Average Marks\n";
    cout << "  8. Display Topper\n";
    cout << "  9. Exit\n";
    cout << "=====================================\n";
}

// ============================================================
//  main(): menu loop using switch-case
// ============================================================
int main() {
    loadFromFile();                             // load saved records at startup
    int choice;

    do {
        showMenu();
        choice = readInt("Enter Your Choice: ", 1, 9);

        switch (choice) {
            case 1: addStudent();       break;
            case 2: displayAll();       break;
            case 3: searchStudent();    break;
            case 4: deleteStudent();    break;
            case 5: updateStudent();    break;
            case 6: sortByMarks();      break;
            case 7: calculateAverage(); break;
            case 8: displayTopper();    break;
            case 9: cout << "\nThank you for using the system. Goodbye!\n"; break;
        }
        if (choice != 9) pause();
    } while (choice != 9);

    return 0;
}

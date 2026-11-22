// ============================================================
//  Student Record Management System - C++ BACKEND (Web Server)
//
//  This program runs a tiny web server on http://localhost:8080
//  It does two jobs:
//    1. Serves the frontend files (HTML, CSS, JS) from /frontend
//    2. Provides a REST API that the frontend calls to manage students
//
//  The DSA logic is the same as the console project:
//    Array, Structure, Linear Search, Bubble Sort,
//    Insertion, Deletion, Traversal.
//  Records are saved permanently in students.txt.
//
//  API (all data is exchanged as JSON):
//    GET    /api/students          -> list all students (traversal)
//    GET    /api/students/{roll}   -> search one student (linear search)
//    POST   /api/students          -> add a student (insertion)
//    PUT    /api/students/{roll}   -> update a student
//    DELETE /api/students/{roll}   -> delete a student (deletion)
//    POST   /api/sort              -> sort by marks (bubble sort)
//    GET    /api/stats             -> count, average and topper(s)
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
using namespace std;

// ---------- Socket setup: works on Windows and Linux/Mac ----------
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET closesocket
    #define INVALID_SOCK INVALID_SOCKET
    #define SEND_FLAGS 0
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socket_t;
    #define CLOSE_SOCKET close
    #define INVALID_SOCK (-1)
    #define SEND_FLAGS MSG_NOSIGNAL
#endif

const int PORT = 8080;
const int MAX_STUDENTS = 100;
const string FILE_NAME = "students.txt";
const string FRONTEND_DIR = "frontend";

// ---------- Structure: one student ----------
struct Student {
    int rollNo;
    string name;
    string course;
    float marks;
};

// ---------- Array: all students ----------
Student students[MAX_STUDENTS];
int studentCount = 0;

// ============================================================
//  File handling (same format as the console version)
//  rollNo|name|course|marks
// ============================================================
void saveToFile() {
    ofstream file(FILE_NAME.c_str());
    for (int i = 0; i < studentCount; i++) {
        file << students[i].rollNo << "|" << students[i].name << "|"
             << students[i].course << "|" << students[i].marks << "\n";
    }
}

void loadFromFile() {
    ifstream file(FILE_NAME.c_str());
    if (!file) return;
    string line;
    while (getline(file, line) && studentCount < MAX_STUDENTS) {
        if (line.empty()) continue;
        stringstream ss(line);
        string roll, name, course, marks;
        if (getline(ss, roll, '|') && getline(ss, name, '|') &&
            getline(ss, course, '|') && getline(ss, marks, '|')) {
            Student s;
            s.rollNo = atoi(roll.c_str());
            s.name = name;
            s.course = course;
            s.marks = (float)atof(marks.c_str());
            students[studentCount++] = s;
        }
    }
}

// ============================================================
//  DSA operations
// ============================================================

// LINEAR SEARCH: returns index of the roll number, or -1
int findStudentIndex(int rollNo) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNo == rollNo) return i;
    }
    return -1;
}

// DELETION: shift all later elements one step left
void removeAt(int index) {
    for (int i = index; i < studentCount - 1; i++) {
        students[i] = students[i + 1];
    }
    studentCount--;
}

// BUBBLE SORT: highest marks first
void bubbleSortByMarks() {
    for (int i = 0; i < studentCount - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < studentCount - i - 1; j++) {
            if (students[j].marks < students[j + 1].marks) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// ============================================================
//  Small JSON helpers (no library needed)
// ============================================================

// Makes text safe to put inside a JSON string
string jsonEscape(const string &text) {
    string out;
    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

string floatToString(float value) {
    ostringstream ss;
    ss << fixed << setprecision(2) << value;
    return ss.str();
}

string studentToJson(const Student &s) {
    return "{\"rollNo\":" + to_string(s.rollNo) +
           ",\"name\":\"" + jsonEscape(s.name) +
           "\",\"course\":\"" + jsonEscape(s.course) +
           "\",\"marks\":" + floatToString(s.marks) + "}";
}

string errorJson(const string &message) {
    return "{\"error\":\"" + jsonEscape(message) + "\"}";
}

// Reads the value of "key" from a simple flat JSON object.
// Works for both  "name":"Aarav"  and  "marks":88.5
bool getJsonField(const string &body, const string &key, string &value) {
    string pattern = "\"" + key + "\"";
    size_t pos = body.find(pattern);
    if (pos == string::npos) return false;
    pos = body.find(':', pos + pattern.size());
    if (pos == string::npos) return false;
    pos++;
    while (pos < body.size() && (body[pos] == ' ' || body[pos] == '\n' || body[pos] == '\t')) pos++;
    if (pos >= body.size()) return false;

    value = "";
    if (body[pos] == '"') {                       // text value
        pos++;
        while (pos < body.size() && body[pos] != '"') {
            if (body[pos] == '\\' && pos + 1 < body.size()) {
                pos++;
                char e = body[pos];
                if (e == 'n') value += '\n';
                else if (e == 't') value += '\t';
                else value += e;
            } else {
                value += body[pos];
            }
            pos++;
        }
        return true;
    }
    while (pos < body.size() && body[pos] != ',' && body[pos] != '}') {  // number value
        value += body[pos];
        pos++;
    }
    size_t end = value.find_last_not_of(" \n\r\t");
    if (end != string::npos) value = value.substr(0, end + 1);
    return true;
}

// ============================================================
//  Input validation
// ============================================================
bool parseInt(const string &text, int &value) {
    stringstream ss(text);
    char extra;
    return (ss >> value) && !(ss >> extra);
}

bool parseFloat(const string &text, float &value) {
    stringstream ss(text);
    char extra;
    return (ss >> value) && !(ss >> extra);
}

// Trims spaces and checks a text field. Returns an error message or "".
string cleanText(string &text, const string &label) {
    size_t first = text.find_first_not_of(" \t");
    size_t last = text.find_last_not_of(" \t");
    if (first == string::npos) return label + " cannot be empty.";
    text = text.substr(first, last - first + 1);
    if (text.find('|') != string::npos) return label + " cannot contain the '|' character.";
    if (text.size() > 50) return label + " is too long (max 50 characters).";
    return "";
}

// Reads name, course and marks from the request body and validates them.
string readDetails(const string &body, Student &s) {
    string name, course, marks;
    if (!getJsonField(body, "name", name)) return "Name is required.";
    if (!getJsonField(body, "course", course)) return "Course is required.";
    if (!getJsonField(body, "marks", marks)) return "Marks are required.";

    string error = cleanText(name, "Name");
    if (error != "") return error;
    error = cleanText(course, "Course");
    if (error != "") return error;

    float m;
    if (!parseFloat(marks, m) || m < 0 || m > 100) return "Marks must be a number between 0 and 100.";

    s.name = name;
    s.course = course;
    s.marks = m;
    return "";
}

// ============================================================
//  API handlers: each one returns the response body
//  and sets the HTTP status code
// ============================================================
string apiGetAll(int &status) {
    // TRAVERSAL: visit every student
    string json = "[";
    for (int i = 0; i < studentCount; i++) {
        if (i > 0) json += ",";
        json += studentToJson(students[i]);
    }
    json += "]";
    status = 200;
    return json;
}

string apiGetOne(int roll, int &status) {
    int index = findStudentIndex(roll);
    if (index == -1) {
        status = 404;
        return errorJson("Student with roll number " + to_string(roll) + " not found.");
    }
    status = 200;
    return studentToJson(students[index]);
}

string apiAdd(const string &body, int &status) {
    if (studentCount >= MAX_STUDENTS) {
        status = 400;
        return errorJson("List is full. Cannot add more students.");
    }
    string rollText;
    int roll;
    if (!getJsonField(body, "rollNo", rollText) || !parseInt(rollText, roll) || roll < 1000000 || roll > 9999999) {
        status = 400;
        return errorJson("Roll number must be exactly 7 digits.");
    }
    // Prevent duplicate roll numbers (linear search)
    if (findStudentIndex(roll) != -1) {
        status = 409;
        return errorJson("Roll number " + to_string(roll) + " already exists.");
    }
    Student s;
    s.rollNo = roll;
    string error = readDetails(body, s);
    if (error != "") {
        status = 400;
        return errorJson(error);
    }
    // INSERTION: add at the end of the array
    students[studentCount++] = s;
    saveToFile();
    status = 201;
    return studentToJson(s);
}

string apiUpdate(int roll, const string &body, int &status) {
    int index = findStudentIndex(roll);
    if (index == -1) {
        status = 404;
        return errorJson("Student with roll number " + to_string(roll) + " not found.");
    }
    Student s;
    s.rollNo = roll;                              // roll number cannot change
    string error = readDetails(body, s);
    if (error != "") {
        status = 400;
        return errorJson(error);
    }
    students[index] = s;
    saveToFile();
    status = 200;
    return studentToJson(s);
}

string apiDelete(int roll, int &status) {
    int index = findStudentIndex(roll);
    if (index == -1) {
        status = 404;
        return errorJson("Student with roll number " + to_string(roll) + " not found.");
    }
    removeAt(index);
    saveToFile();
    status = 200;
    return "{\"message\":\"Student deleted.\"}";
}

string apiSort(int &status) {
    bubbleSortByMarks();
    saveToFile();
    return apiGetAll(status);
}

string apiStats(int &status) {
    float total = 0, highest = 0;
    for (int i = 0; i < studentCount; i++) {
        total += students[i].marks;
        if (i == 0 || students[i].marks > highest) highest = students[i].marks;
    }
    float average = (studentCount > 0) ? total / studentCount : 0;

    string toppers = "[";
    bool first = true;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].marks == highest) {       // handles ties
            if (!first) toppers += ",";
            toppers += studentToJson(students[i]);
            first = false;
        }
    }
    toppers += "]";

    status = 200;
    return "{\"count\":" + to_string(studentCount) +
           ",\"average\":" + floatToString(average) +
           ",\"toppers\":" + toppers + "}";
}

// ============================================================
//  HTTP server code
// ============================================================
string statusText(int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 409: return "Conflict";
        default:  return "Internal Server Error";
    }
}

void sendAll(socket_t sock, const string &data) {
    size_t sent = 0;
    while (sent < data.size()) {
        int n = (int)send(sock, data.c_str() + sent, (int)(data.size() - sent), SEND_FLAGS);
        if (n <= 0) return;
        sent += n;
    }
}

void sendResponse(socket_t sock, int status, const string &contentType, const string &body) {
    string response = "HTTP/1.1 " + to_string(status) + " " + statusText(status) + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + to_string(body.size()) + "\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
    response += "Access-Control-Allow-Headers: Content-Type\r\n";
    response += "Cache-Control: no-store\r\n";
    response += "Connection: close\r\n\r\n";
    response += body;
    sendAll(sock, response);
}

string getContentType(const string &path) {
    size_t dot = path.rfind('.');
    string ext = (dot == string::npos) ? "" : path.substr(dot);
    if (ext == ".html") return "text/html; charset=utf-8";
    if (ext == ".css") return "text/css; charset=utf-8";
    if (ext == ".js") return "application/javascript; charset=utf-8";
    if (ext == ".json") return "application/json";
    if (ext == ".png") return "image/png";
    if (ext == ".svg") return "image/svg+xml";
    return "application/octet-stream";
}

// Sends a file from the frontend folder
void serveStaticFile(socket_t sock, string path) {
    if (path == "/") path = "/index.html";
    if (path.find("..") != string::npos) {        // block access to other folders
        sendResponse(sock, 404, "text/plain", "Not found");
        return;
    }
    ifstream file((FRONTEND_DIR + path).c_str(), ios::binary);
    if (!file) {
        sendResponse(sock, 404, "text/plain", "404 - File not found");
        return;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    sendResponse(sock, 200, getContentType(path), buffer.str());
}

// Decides what to do for each request
void handleRequest(socket_t sock, const string &method, string path, const string &body) {
    size_t q = path.find('?');                    // remove ?query part
    if (q != string::npos) path = path.substr(0, q);

    if (method == "OPTIONS") {
        sendResponse(sock, 204, "text/plain", "");
        return;
    }

    const string base = "/api/students";
    int status = 200;
    string result;

    if (path == base) {
        if (method == "GET") result = apiGetAll(status);
        else if (method == "POST") result = apiAdd(body, status);
        else { status = 405; result = errorJson("Method not allowed."); }
    }
    else if (path.compare(0, base.size() + 1, base + "/") == 0) {
        int roll;
        if (!parseInt(path.substr(base.size() + 1), roll)) {
            status = 400;
            result = errorJson("Roll number in the URL must be a number.");
        }
        else if (method == "GET") result = apiGetOne(roll, status);
        else if (method == "PUT") result = apiUpdate(roll, body, status);
        else if (method == "DELETE") result = apiDelete(roll, status);
        else { status = 405; result = errorJson("Method not allowed."); }
    }
    else if (path == "/api/sort" && method == "POST") result = apiSort(status);
    else if (path == "/api/stats" && method == "GET") result = apiStats(status);
    else if (path.compare(0, 5, "/api/") == 0) {
        status = 404;
        result = errorJson("Unknown API route.");
    }
    else if (method == "GET") {
        serveStaticFile(sock, path);              // not an API call: send a web page file
        return;
    }
    else { status = 405; result = errorJson("Method not allowed."); }

    sendResponse(sock, status, "application/json", result);
}

// Reads one full HTTP request from the browser
void handleClient(socket_t client) {
    string request;
    char buffer[4096];
    size_t headerEnd = string::npos;

    // read until the end of the headers
    while (headerEnd == string::npos && request.size() < 1000000) {
        int n = (int)recv(client, buffer, sizeof(buffer), 0);
        if (n <= 0) return;
        request.append(buffer, n);
        headerEnd = request.find("\r\n\r\n");
    }
    if (headerEnd == string::npos) return;

    string headers = request.substr(0, headerEnd);
    string body = request.substr(headerEnd + 4);

    // find Content-Length (case-insensitive) and read the rest of the body
    string lower = headers;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    size_t pos = lower.find("content-length:");
    size_t contentLength = 0;
    if (pos != string::npos) contentLength = (size_t)atoi(headers.c_str() + pos + 15);
    if (contentLength > 100000) contentLength = 100000;
    while (body.size() < contentLength) {
        int n = (int)recv(client, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        body.append(buffer, n);
    }

    // first line looks like:  GET /api/students HTTP/1.1
    stringstream firstLine(headers.substr(0, headers.find("\r\n")));
    string method, path, version;
    firstLine >> method >> path >> version;

    handleRequest(client, method, path, body);
}

int main() {
    loadFromFile();

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Could not start Winsock.\n";
        return 1;
    }
#endif

    socket_t server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCK) {
        cout << "Could not create the server socket.\n";
        return 1;
    }
    int yes = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");   // only this computer can connect
    address.sin_port = htons(PORT);
    for (size_t i = 0; i < sizeof(address.sin_zero); i++) address.sin_zero[i] = 0;

    if (bind(server, (sockaddr *)&address, sizeof(address)) < 0) {
        cout << "Could not use port " << PORT << ". Is the server already running?\n";
        return 1;
    }
    listen(server, 10);

    cout << "=====================================\n";
    cout << "  Student Record Management Server\n";
    cout << "=====================================\n";
    cout << "  Loaded " << studentCount << " student(s) from " << FILE_NAME << "\n";
    cout << "  Open in your browser: http://localhost:" << PORT << "\n";
    cout << "  Press Ctrl + C to stop the server.\n";

    // Main loop: wait for a browser request, handle it, repeat
    while (true) {
        socket_t client = accept(server, NULL, NULL);
        if (client == INVALID_SOCK) continue;
        handleClient(client);
        CLOSE_SOCKET(client);
    }
    return 0;
}

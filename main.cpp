#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

#include "Student.h"
#include "DynamicArray.h"

using namespace std;

bool validStudent(const Student& student) {
    return !student.regNumber.empty() &&
           !student.name.empty() &&
           student.marks >= 0.0 &&
           student.marks <= 100.0;
}

bool registrationExists(const DynamicArray<Student>& students,
                        const string& regNumber) {
    Student current;
    for (int i = 0; i < students.size(); ++i) {
        students.get(i, current);
        if (current.regNumber == regNumber) {
            return true;
        }
    }
    return false;
}

int loadStudents(DynamicArray<Student>& students,
                 const string& filename) {
    ifstream input(filename);
    if (!input) {
        cout << "Could not open " << filename
             << ". Starting with an empty register.\n";
        return 0;
    }

    string line;
    int loaded = 0;

    while (getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream row(line);
        Student student;
        string marksText;

        if (!getline(row, student.regNumber, ',') ||
            !getline(row, student.name, ',') ||
            !getline(row, marksText)) {
            continue;
        }

        try {
            size_t used = 0;
            student.marks = stod(marksText, &used);

            if (used != marksText.size() ||
                !validStudent(student) ||
                registrationExists(students, student.regNumber)) {
                continue;
            }

            students.add(student);
            ++loaded;
        } catch (...) {
            // Skip malformed records instead of crashing.
        }
    }

    return loaded;
}

bool saveStudents(const DynamicArray<Student>& students,
                  const string& filename) {
    ofstream output(filename);
    if (!output) {
        return false;
    }

    Student student;
    for (int i = 0; i < students.size(); ++i) {
        students.get(i, student);
        output << student.regNumber << ','
               << student.name << ','
               << student.marks << '\n';
    }

    return output.good();
}

void displayStudents(const DynamicArray<Student>& students) {
    if (students.isEmpty()) {
        cout << "No student records are available.\n";
        return;
    }

    cout << "\nREGISTRATION\tNAME\t\tMARKS\n";
    cout << "------------------------------------------\n";

    Student student;
    for (int i = 0; i < students.size(); ++i) {
        students.get(i, student);
        cout << student.regNumber << '\t'
             << student.name << "\t\t"
             << student.marks << '\n';
    }
}

void addStudent(DynamicArray<Student>& students) {
    Student student;

    cout << "Enter registration number: ";
    cin >> student.regNumber;

    if (registrationExists(students, student.regNumber)) {
        cout << "That registration number already exists.\n";
        return;
    }

    cout << "Enter student name: ";
    getline(cin >> ws, student.name);

    cout << "Enter marks (0-100): ";
    if (!(cin >> student.marks)) {
        cout << "Invalid marks. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    if (!validStudent(student)) {
        cout << "Invalid record. Name and registration are required; "
                "marks must be from 0 to 100.\n";
        return;
    }

    students.add(student);
    cout << "Student added.\n";
}

void updateStudent(DynamicArray<Student>& students) {
    string regNumber;
    cout << "Enter registration number to update: ";
    cin >> regNumber;

    Student student;
    for (int i = 0; i < students.size(); ++i) {
        students.get(i, student);
        if (student.regNumber == regNumber) {
            cout << "Enter new marks (0-100): ";
            double newMarks;

            if (!(cin >> newMarks)) {
                cout << "Invalid marks input.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return;
            }

            if (newMarks < 0.0 || newMarks > 100.0) {
                cout << "Marks must be between 0 and 100.\n";
                return;
            }

            student.marks = newMarks;
            students.update(i, student);
            cout << "Student result updated.\n";
            return;
        }
    }

    cout << "Student not found.\n";
}

void removeStudent(DynamicArray<Student>& students) {
    string regNumber;
    cout << "Enter registration number to remove: ";
    cin >> regNumber;

    Student student;
    for (int i = 0; i < students.size(); ++i) {
        students.get(i, student);
        if (student.regNumber == regNumber) {
            students.remove(i);
            cout << "Student removed.\n";
            return;
        }
    }

    cout << "Student not found.\n";
}

int main() {
    const string filename = "students.txt";
    DynamicArray<Student> students(2);

    int loaded = loadStudents(students, filename);
    cout << loaded << " student record(s) loaded.\n";

    int choice = -1;
    do {
        cout << "\n===== STUDENT RESULTS REGISTER =====\n"
             << "1. Add student\n"
             << "2. Display all students\n"
             << "3. Update student marks\n"
             << "4. Remove student\n"
             << "5. Save records\n"
             << "6. Show array size and capacity\n"
             << "0. Save and exit\n"
             << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Please enter a menu number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                addStudent(students);
                break;
            case 2:
                displayStudents(students);
                break;
            case 3:
                updateStudent(students);
                break;
            case 4:
                removeStudent(students);
                break;
            case 5:
                cout << (saveStudents(students, filename)
                         ? "Records saved successfully.\n"
                         : "Could not save records.\n");
                break;
            case 6:
                cout << "Current size: " << students.size()
                     << "\nCurrent capacity: "
                     << students.capacity() << '\n';
                break;
            case 0:
                if (saveStudents(students, filename)) {
                    cout << "Records saved. Program closed.\n";
                } else {
                    cout << "Warning: records could not be saved.\n";
                }
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);

    return 0;
}

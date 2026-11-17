STUDENT RESULTS REGISTER - A1

Files:
- Student.h: defines the Student record structure.
- Student.cpp: separate Student implementation file.
- DynamicArray.h: templated dynamic array with capacity doubling.
- main.cpp: menu, record operations, and file loading/saving.
- students.txt: sample student records.

Compile from this folder using:
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Student.cpp -o StudentResults

Run in Windows PowerShell:
.\StudentResults.exe

Run in MSYS2 / MinGW terminal:
./StudentResults.exe

The program reads students.txt when it starts and saves updated records
when option 5 is selected or when the user exits with option 0.

A1 implementation notes:
- The array is implemented using a raw dynamically allocated array.
- Capacity doubles whenever the array becomes full.
- The class has a destructor, deep copy constructor, and deep copy
  assignment operator.
- Invalid array indexes are rejected safely using bool return values.
- Student marks are checked to be between 0 and 100.
- Duplicate registration numbers are rejected.

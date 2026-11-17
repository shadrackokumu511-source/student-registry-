#ifndef STUDENT_H
#define STUDENT_H

#include <string>

// A student record stored in the dynamic array.
struct Student {
    std::string regNumber;
    std::string name;
    double marks = 0.0;
};

#endif

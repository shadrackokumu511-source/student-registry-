#include <iostream>
#include "DynamicArray.h"
#include "sorting.h"

static void show(const char* title, const DynamicArray<StudentRecord>& r) {
    std::cout << title << "\n";
    for (int i = 0; i < r.size(); ++i)
        std::cout << "  " << r.at(i).id << "  " << r.at(i).name << "  " << r.at(i).score << "\n";
}

int main() {
    try {
        DynamicArray<StudentRecord> a;
        a.loadFromFile("student.txt");
        DynamicArray<StudentRecord> b(a);
        insertionSort(a.raw(), a.size(), ByScore{});
        show("insertionSort by score:", a);
        fastSort(b.raw(), b.size(), ByScore{});
        show(USE_MERGE ? "mergeSort by score:" : "quickSortRand by score:", b);
        for (double k : {75.0, 999.0}) {
            int idx = binarySearch(a.raw(), a.size(), k, ScoreKeyLess{});
            std::cout << "binarySearch(" << k << ") -> "
                      << (idx >= 0 ? "found at index " + std::to_string(idx) : "absent") << "\n";
        }
    } catch (const std::exception& e) { std::cerr << "Error: " << e.what() << "\n"; return 1; }
}

// =====================================================================
//  FILE: StudentResultsRegister.cpp
//  SCENARIO 6: Student Results Register
//  PRIMARY DATA STRUCTURE (Task A1): Dynamic array with capacity doubling
//  COURSE: C++17, compiles with g++ / clang++ (no STL containers used in A1)
//
//  Build:  g++ -std=c++17 -Wall -Wextra -fsanitize=address StudentResultsRegister.cpp -o register
//  Run:    ./register
// =====================================================================

#include <iostream>      // std::cout, std::cerr, std::cin — console I/O
#include <fstream>       // std::ifstream, std::ofstream — file load/save (A1 requirement)
#include <string>        // std::string, std::getline — student name field
#include <sstream>       // std::istringstream — parse one CSV line
#include <stdexcept>     // std::out_of_range, std::invalid_argument — safe rejection (A1)
#include <chrono>        // std::chrono::high_resolution_clock — timing experiment (A4)
#include <random>        // std::mt19937 — random pivot for quicksort (A2, odd reg. no.)
#include <algorithm>     // std::swap, std::stable_sort — swap helper + comparison test only
#include <cstdlib>       // std::atoi — not strictly needed; kept for portability
#include <iomanip>       // std::setw, std::fixed — formatted output tables

// ---------------------------------------------------------------------
// 1. RECORD TYPE (A1: "store records of a struct type that you define")
// ---------------------------------------------------------------------
struct StudentRecord {          // One row of the student results register
    int    id;                  // Unique student ID (search/sort key candidate)
    std::string name;           // Student full name
    std::string course;         // Course code, e.g. "CS101"
    double score;               // Final mark 0..100 (meaningful sort key)
};

// ---------------------------------------------------------------------
// 2. PRIMARY DATA STRUCTURE (A1): DynamicArray<T>
//    - Own class template in a header-style section (single file for submission)
//    - Manual memory management: new[] / delete[]
//    - Capacity doubling on push_back
//    - Deep copy constructor + destructor  (rule of three)
// ---------------------------------------------------------------------
template <typename T>
class DynamicArray {
private:
    T*      data;               // Pointer to heap block holding the records
    int     count;              // Number of records currently stored
    int     capacity;           // Allocated slots in the heap block

    // Grow the buffer to double its size. Called only when full.
    void grow() {
        int newCap = (capacity == 0) ? 1 : capacity * 2;   // doubling (or start at 1)
        T* newData = new T[newCap];                        // allocate new block
        for (int i = 0; i < count; ++i)                    // copy existing records
            newData[i] = data[i];                          // shallow copy of POD-ish struct
        delete[] data;                                     // free old block (no leak)
        data     = newData;                                // point to new block
        capacity = newCap;                                 // update capacity
    }

public:
    // --- Constructor: start empty, no allocation yet ---
    DynamicArray() : data(nullptr), count(0), capacity(0) {}

         // --- Deep copy constructor (A1 requirement) ---
    DynamicArray(const DynamicArray& other)
        : data(nullptr), count(other.count), capacity(other.capacity) {
        if (capacity > 0) {
            data = new T[capacity];                        // allocate same capacity
            for (int i = 0; i < count; ++i)
                data[i] = other.data[i];                   // deep copy each element
        }
    }

    // --- Copy assignment (rule of three: avoids double-free) ---
    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this;                  // self-assignment guard
        T* newData = nullptr;
        if (other.capacity > 0) {
            newData = new T[other.capacity];               // allocate first (exception safety)
            for (int i = 0; i < other.count; ++i)
                newData[i] = other.data[i];
        }
        delete[] data;                                     // free old memory
        data     = newData;                                // adopt new block
        count    = other.count;
        capacity = other.capacity;
        return *this;
    }

    // --- Destructor (A1 requirement: correct memory management) ---
    ~DynamicArray() {
        delete[] data;                                     // free heap block once
        data = nullptr;                                    // avoid dangling pointer
    }

    // --- Append with capacity doubling ---
    void push_back(const T& value) {
        if (count == capacity) grow();                     // double when full
        data[count++] = value;                             // store and advance count
    }

    // --- Remove at index; shift left; reject invalid index safely ---
    void removeAt(int index) {
        if (index < 0 || index >= count)                   // boundary check
            throw std::out_of_range("removeAt: index out of range");
        for (int i = index; i < count - 1; ++i)            // shift elements left
            data[i] = data[i + 1];
        --count;                                           // one fewer record
        // Shrink when very sparse (optional but good practice; keeps capacity >= 1)
        if (capacity > 4 && count < capacity / 4) {
            int newCap = capacity / 2;
            T* newData = new T[newCap];
            for (int i = 0; i < count; ++i) newData[i] = data[i];
            delete[] data;
            data = newData;
            capacity = newCap;
        }
    }

    // --- Element access with bounds checking (safe rejection) ---
    T& at(int index) {
        if (index < 0 || index >= count)
            throw std::out_of_range("at: index out of range");
        return data[index];
    }
    const T& at(int index) const {                         // const overload for read-only use
        if (index < 0 || index >= count)
            throw std::out_of_range("at: index out of range");
        return data[index];
    }

    // --- Simple accessors ---
    int size()     const { return count; }                 // O(1)
    int getCapacity() const { return capacity; }           // O(1), used in tests
    bool empty()   const { return count == 0; }            // O(1)

    // --- Linear search by predicate (A1 search helper) ---
    template <typename Pred>
    int findIndex(Pred pred) const {
        for (int i = 0; i < count; ++i)                    // scan all records
            if (pred(data[i])) return i;                   // first match
        return -1;                                         // not found
    }

    // --- Load records from a text file (CSV: id,name,course,score) ---
    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);                        // open for reading
        if (!in) throw std::runtime_error("cannot open input file: " + filename);
        std::string line;
        while (std::getline(in, line)) {                   // read one CSV line
            if (line.empty()) continue;                    // skip blank lines
            std::istringstream ss(line);                   // parse fields
            std::string idStr, name, course, scoreStr;
            if (!std::getline(ss, idStr,   ',')) continue;
            if (!std::getline(ss, name,    ',')) continue;
            if (!std::getline(ss, course,  ',')) continue;
            if (!std::getline(ss, scoreStr,',')) continue;
            StudentRecord r;                               // build one record
            r.id     = std::stoi(idStr);                   // may throw on bad data (safe)
            r.name   = name;
            r.course = course;
            r.score  = std::stod(scoreStr);
            push_back(r);                                  // append to dynamic array
        }
    }

    // --- Save records back to the same format ---
    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);                       // open for writing (truncates)
        if (!out) throw std::runtime_error("cannot open output file: " + filename);
        for (int i = 0; i < count; ++i) {                  // write every record
            out << data[i].id << ',' << data[i].name << ','
                << data[i].course << ',' << data[i].score << '\n';
        }
    }

    // --- Raw pointer for sorting/searching helpers (internal use) ---
    T*       raw()       { return data; }
    const T* raw() const { return data; }
};

// ---------------------------------------------------------------------
// 3. SORTING (A2): templated sorts taking a comparator
//    - insertionSort  : required (stable, O(n^2) worst)
//    - mergeSort      : required if last reg. digit is EVEN (stable, O(n log n))
//    - quickSortRand  : required if last reg. digit is ODD  (random pivot, O(n log n) avg)
// ---------------------------------------------------------------------

// Insertion sort — stable because equal keys are never swapped past each other.
template <typename T, typename Compare>
void insertionSort(T* a, int n, Compare less) {
    for (int i = 1; i < n; ++i) {                          // start from second element
        T key = a[i];                                      // element to insert
        int j = i - 1;
        while (j >= 0 && less(key, a[j])) {                // shift larger elements right
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;                                    // place key in correct slot
    }
}

// Merge sort helpers — stable because left run wins ties.
template <typename T, typename Compare>
void mergeRuns(T* a, T* tmp, int lo, int mid, int hi, Compare less) {
    int i = lo, j = mid + 1, k = lo;                       // i: left, j: right, k: output
    while (i <= mid && j <= hi) {                          // merge two sorted halves
        if (less(a[j], a[i])) tmp[k++] = a[j++];           // right strictly smaller
        else                  tmp[k++] = a[i++];           // left wins ties -> stable
    }
    while (i <= mid) tmp[k++] = a[i++];                    // drain left
    while (j <= hi)  tmp[k++] = a[j++];                    // drain right
    for (int t = lo; t <= hi; ++t) a[t] = tmp[t];          // copy back
}

template <typename T, typename Compare>
void mergeSortRec(T* a, T* tmp, int lo, int hi, Compare less) {
    if (lo >= hi) return;                                  // 0/1 element: sorted
    int mid = lo + (hi - lo) / 2;                          // avoid overflow
    mergeSortRec(a, tmp, lo, mid, less);                   // sort left half
    mergeSortRec(a, tmp, mid + 1, hi, less);               // sort right half
    mergeRuns(a, tmp, lo, mid, hi, less);                  // merge
}

template <typename T, typename Compare>
void mergeSort(T* a, int n, Compare less) {
    if (n < 2) return;                                     // nothing to do
    T* tmp = new T[n];                                     // scratch buffer (O(n) space)
    mergeSortRec(a, tmp, 0, n - 1, less);                  // recursive sort
    delete[] tmp;                                          // free scratch (no leak)
}

// Quicksort with random pivot — expected O(n log n), NOT stable.
template <typename T, typename Compare>
int partitionRand(T* a, int lo, int hi, Compare less, std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(lo, hi);       // inclusive range
    int p = dist(rng);                                     // random pivot index
    std::swap(a[p], a[hi]);                                // move pivot to end
    T pivot = a[hi];
    int i = lo - 1;                                        // boundary of < pivot region
    for (int j = lo; j < hi; ++j) {
        if (less(a[j], pivot)) {                           // strictly less than pivot
            ++i;
            std::swap(a[i], a[j]);
        }
    }
    std::swap(a[i + 1], a[hi]);                            // place pivot in final slot
    return i + 1;                                          // return pivot position
}

template <typename T, typename Compare>
void quickSortRec(T* a, int lo, int hi, Compare less, std::mt19937& rng) {
    if (lo >= hi) return;                                  // base case
    int p = partitionRand(a, lo, hi, less, rng);           // split around random pivot
    quickSortRec(a, lo, p - 1, less, rng);                 // sort left partition
    quickSortRec(a, p + 1, hi, less, rng);                 // sort right partition
}

template <typename T, typename Compare>
void quickSortRand(T* a, int n, Compare less) {
    if (n < 2) return;                                     // nothing to do
    std::mt19937 rng(12345);                               // fixed seed -> reproducible timings
    quickSortRec(a, 0, n - 1, less, rng);                  // start recursion
}

// ---------------------------------------------------------------------
// 4. SEARCHING (A2): iterative binary search on a sorted array
//    Returns index of first record with matching key, or -1.
//    Precondition: array is sorted by the same comparator `less` used here.
// ---------------------------------------------------------------------
template <typename T, typename Key, typename Compare>
int binarySearch(const T* a, int n, const Key& key, Compare less) {
    int lo = 0, hi = n - 1;                                // inclusive search window
    while (lo <= hi) {                                     // iterative (no recursion)
        int mid = lo + (hi - lo) / 2;                      // overflow-safe midpoint
        if (less(a[mid], key))      lo = mid + 1;          // key is to the right
        else if (less(key, a[mid])) hi = mid - 1;          // key is to the left
        else                        return mid;            // match found
    }
    return -1;                                             // absent key
}

// ---------------------------------------------------------------------
// 5. COMPARATORS (A2: "so that records can be sorted by any field")
// ---------------------------------------------------------------------
struct ById    { bool operator()(const StudentRecord& x, const StudentRecord& y) const { return x.id    < y.id;    } };
struct ByScore { bool operator()(const StudentRecord& x, const StudentRecord& y) const { return x.score < y.score; } };
struct ByName  { bool operator()(const StudentRecord& x, const StudentRecord& y) const { return x.name  < y.name;  } };

// ---------------------------------------------------------------------
// 6. MAIN — A3 test evidence + A4 timing experiment
// ---------------------------------------------------------------------
int main() {
    // === Pick your O(n log n) sort by the LAST DIGIT of your reg. number ===
    const int REG_LAST_DIGIT = 3;          // <-- CHANGE to your own last digit
    const bool USE_MERGE = (REG_LAST_DIGIT % 2 == 0);   // even -> merge, odd -> quick

    try {
        // ---------- A1: load at least 10 sample records ----------
        DynamicArray<StudentRecord> reg;                    // empty structure (boundary case)
        reg.loadFromFile("students.txt");                   // must contain >= 10 rows
        std::cout << "Loaded " << reg.size()
                  << " records (capacity " << reg.getCapacity() << ")\n";

        // ---------- A2: sort by meaningful key (score) ----------
        insertionSort(reg.raw(), reg.size(), ByScore{});    // stable sort by score
        std::cout << "After insertionSort (by score):\n";
        for (int i = 0; i < reg.size(); ++i)
            std::cout << "  " << reg.at(i).id << "  " << reg.at(i).name
                      << "  " << reg.at(i).score << "\n";

        // Binary search on sorted array (by score key)
        double probe = 75.0;                                // key to find
        int idx = binarySearch(reg.raw(), reg.size(), probe,
                               [](const StudentRecord& a, double k){ return a.score < k; });
        std::cout << "binarySearch(score=" << probe << ") -> "
                  << (idx >= 0 ? "found at index " + std::to_string(idx) : "absent") << "\n";

        // Absent-key boundary case
        probe = 999.0;
        idx = binarySearch(reg.raw(), reg.size(), probe,
                           [](const StudentRecord& a, double k){ return a.score < k; });
        std::cout << "binarySearch(score=" << probe << ") -> "
                  << (idx >= 0 ? "found" : "absent (expected)") << "\n";

        // ---------- A2: O(n log n) sort chosen by reg. digit ----------
        if (USE_MERGE) {
            std::cout << "Chosen O(n log n) sort: mergeSort\n";
            mergeSort(reg.raw(), reg.size(), ByScore{});
        } else {
            std::cout << "Chosen O(n log n) sort: quickSortRand (random pivot)\n";
            quickSortRand(reg.raw(), reg.size(), ByScore{});
        }

        // ---------- A1: save updated records back ----------
        reg.saveToFile("students_out.txt");
        std::cout << "Saved updated records to students_out.txt\n";

        // ---------- A4: timing experiment n = 1000..64000, 3 runs each ----------
        std::cout << "\n=== A4 TIMING: insertionSort vs " 
                  << (USE_MERGE ? "mergeSort" : "quickSortRand") << " ===\n";
        std::mt19937 rng(42);                               // reproducible random data
        std::uniform_real_distribution<double> dist(0.0, 100.0);
        std::cout << std::setw(8) << "n"
                  << std::setw(14) << "insertion(ms)"
                  << std::setw(14) << "nlogn(ms)"
                  << std::setw(10) << "ratio\n";
        double prevIns = 0, prevFast = 0;                   // for doubling-ratio column
        for (int n = 1000; n <= 64000; n *= 2) {
            double insTotal = 0, fastTotal = 0;             // accumulate 3 runs
            for (int run = 0; run < 3; ++run) {
                DynamicArray<StudentRecord> a, b;           // independent copies
                for (int i = 0; i < n; ++i) {
                    StudentRecord r{i, "S", "C", dist(rng)};// synthetic record
                    a.push_back(r);                         // fill A
                    b.push_back(r);                         // fill B identically
                }
                auto t0 = std::chrono::high_resolution_clock::now();
                insertionSort(a.raw(), a.size(), ByScore{});// time insertion sort
                auto t1 = std::chrono::high_resolution_clock::now();
                if (USE_MERGE) mergeSort(b.raw(), b.size(), ByScore{});
                else           quickSortRand(b.raw(), b.size(), ByScore{});
                auto t2 = std::chrono::high_resolution_clock::now();
                insTotal  += std::chrono::duration<double, std::milli>(t1 - t0).count();
                fastTotal += std::chrono::duration<double, std::milli>(t2 - t1).count();
            }
            double insAvg  = insTotal  / 3.0;               // average of 3 runs
            double fastAvg = fastTotal / 3.0;
            double insRatio  = prevIns  > 0 ? insAvg  / prevIns  : 0; // ~4 expected
            double fastRatio = prevFast > 0 ? fastAvg / prevFast : 0; // ~2 expected
            std::cout << std::setw(8) << n
                      << std::setw(14) << std::fixed << std::setprecision(3) << insAvg
                      << std::setw(14) << fastAvg
                      << std::setw(10) << fastRatio << "\n";
            prevIns = insAvg; prevFast = fastAvg;
        }

    } catch (const std::exception& ex) {                    // A1: reject invalid ops safely
        std::cerr << "FATAL: " << ex "\n";
return 1;
}
return 0;
}
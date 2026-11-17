
#ifndef SORTING_H
#define SORTING_H

#include <functional>

// Insertion Sort
template <typename T, typename Compare>
void insertionSort(T arr[], int n, Compare comp) {
    for (int i = 1; i < n; i++) {
        T key = arr[i];
        int j = i - 1;

        while (j >= 0 && comp(key, arr[j])) {
            arr[j + 1] = arr[j];
            j--;
        }

        arr[j + 1] = key;
    }
}

// Merge two sorted sections
template <typename T, typename Compare>
void merge(T arr[], T temp[], int left,
           int mid, int right, Compare comp) {

    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {
        if (comp(arr[j], arr[i])) {
            temp[k++] = arr[j++];
        } else {
            temp[k++] = arr[i++];
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (int x = left; x <= right; x++) {
        arr[x] = temp[x];
    }
}

// Recursive Merge Sort helper
template <typename T, typename Compare>
void mergeSortHelper(T arr[], T temp[],
                     int left, int right, Compare comp) {

    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    mergeSortHelper(arr, temp, left, mid, comp);
    mergeSortHelper(arr, temp, mid + 1, right, comp);

    merge(arr, temp, left, mid, right, comp);
}

// Merge Sort
template <typename T, typename Compare>
void mergeSort(T arr[], int n, Compare comp) {
    if (n <= 1) {
        return;
    }

    T* temp = new T[n];

    mergeSortHelper(arr, temp, 0, n - 1, comp);

    delete[] temp;
}

// Iterative Binary Search
template <typename T, typename Key, typename Compare>
int binarySearch(const T arr[], int n,
                 const Key& key, Compare comp) {

    int low = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (!comp(arr[mid], key) &&
            !comp(key, arr[mid])) {
            return mid;
        }

        if (comp(arr[mid], key)) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

#endif

#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H

#include <stdexcept>

template <typename T>
class DynamicArray {
private:
    T* data;
    int currentSize;
    int currentCapacity;

    void grow() {
        int newCapacity = (currentCapacity == 0) ? 2 : currentCapacity * 2;
        T* newData = new T[newCapacity];

        for (int i = 0; i < currentSize; ++i) {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        currentCapacity = newCapacity;
    }

public:
    explicit DynamicArray(int initialCapacity = 2)
        : data(nullptr), currentSize(0),
          currentCapacity(initialCapacity > 0 ? initialCapacity : 2) {
        data = new T[currentCapacity];
    }

    // Deep copy constructor.
    DynamicArray(const DynamicArray& other)
        : data(nullptr), currentSize(other.currentSize),
          currentCapacity(other.currentCapacity) {
        data = new T[currentCapacity];
        for (int i = 0; i < currentSize; ++i) {
            data[i] = other.data[i];
        }
    }

    // Deep copy assignment operator.
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            T* newData = new T[other.currentCapacity];
            for (int i = 0; i < other.currentSize; ++i) {
                newData[i] = other.data[i];
            }

            delete[] data;
            data = newData;
            currentSize = other.currentSize;
            currentCapacity = other.currentCapacity;
        }
        return *this;
    }

    // Releases dynamically allocated memory.
    ~DynamicArray() {
        delete[] data;
    }

    void add(const T& value) {
        if (currentSize == currentCapacity) {
            grow();
        }
        data[currentSize++] = value;
    }

    bool get(int index, T& result) const {
        if (index < 0 || index >= currentSize) {
            return false;
        }
        result = data[index];
        return true;
    }

    bool update(int index, const T& value) {
        if (index < 0 || index >= currentSize) {
            return false;
        }
        data[index] = value;
        return true;
    }

    bool remove(int index) {
        if (index < 0 || index >= currentSize) {
            return false;
        }

        for (int i = index; i < currentSize - 1; ++i) {
            data[i] = data[i + 1];
        }
        --currentSize;
        return true;
    }

    int size() const {
        return currentSize;
    }

    int capacity() const {
        return currentCapacity;
    }

    bool isEmpty() const {
        return currentSize == 0;
    }
};

#endif

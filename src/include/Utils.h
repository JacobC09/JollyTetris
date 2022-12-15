#include "pch.h"

inline int mod(int a, float b) {  
  return a - floor(a/b)*b;
}

template<typename T>
inline bool find(std::vector<int> vector, T value) {
    for (T otherValue : vector) {
        if (value == otherValue)
            return true;
    }
    return false;
}
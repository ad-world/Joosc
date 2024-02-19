#pragma once
#include <vector>
#include <algorithm>

namespace util{

// Remove all duplicates from vector
template <typename T> void removeDuplicates(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
    auto last = std::unique(vec.begin(), vec.end());
    vec.erase(last, vec.end());
};

};

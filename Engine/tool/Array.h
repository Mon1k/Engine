#pragma once

#include <vector>
#include <random>

class Array
{
public:
    template <typename T>
    T static getRandom(std::vector<T> arr)
    {
        return arr[std::rand() % arr.size()];
    }
};
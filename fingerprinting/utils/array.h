#ifndef ARRAY_H
#define ARRAY_H
#include <iostream>

namespace array
{
    template <typename Iterable>
    Iterable &square(Iterable &input)
    {
        for (auto &i : input)
        {
            i *= i;
        }
        return input;
    }

    template <typename Iterable>
    Iterable add(const Iterable &lhs, const Iterable &rhs)
    {
        Iterable result;
        result.resize(lhs.size());
        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            result[i] = lhs[i] + rhs[i];
        }
        return result;
    }

    template <typename Iterable>
    Iterable& devide(Iterable &lhs, const int rhs)
    {
        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            lhs[i] = lhs[i] / rhs;
        }
        return lhs;
    }

    template <typename Iterable, typename T, int N>
    Iterable &multiply(Iterable &lhs, const T (&arr)[N])
    {
        for (std::size_t i = 0; i < lhs.size(); ++i)
        {
            lhs[i] *= arr[i];
        }
        return lhs;
    }

    template <typename Iterable, typename T>
    Iterable &max(Iterable &lhs, const T &rhs)
    {
        for (auto &i : lhs)
        {
            i = i > rhs ? i : rhs;
        }
        return lhs;
    }

    template <typename Iterable>
    void print(Iterable &input, int limit = -1)
    {
        for (const auto &i : input)
        {
            std::cout << i << ", ";
            if (limit > 0 && --limit == 0)
                break;
        }
        std::cout << std::endl;
    }
} 

#endif // ARRAY_H
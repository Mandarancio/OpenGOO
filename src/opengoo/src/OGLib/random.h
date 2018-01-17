#pragma once

#include <random>

namespace oglib
{
class Random
{
public:
    template<typename T>
    typename std::conditional<std::is_floating_point<T>::value, float, int>::type Range(T aNum1, T aNum2)
    {
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "T is not floating point or integral");

        typedef std::uniform_int_distribution<int> IntDistribution;
        typedef std::uniform_real_distribution<float> RealDistribution;
        typedef typename std::conditional<std::is_floating_point<T>::value, RealDistribution, IntDistribution>::type TDistribution;

        TDistribution distribution(aNum1, aNum2);
        return distribution(mGenerator);
    }

private:
    std::default_random_engine mGenerator;
};
}


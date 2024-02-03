#include <algorithm>
#include <type_traits>
#include <vector>
#include <iostream>

template<typename T>
struct hasSwap
{
    template<typename V>
    static int test(decltype(&V::swap));

    template<typename V>
    static char test(...);

    static constexpr bool value = sizeof(decltype(test<T>(0))) == sizeof(int);
};

template<typename T>
constexpr bool hasSwap_v = hasSwap<T>::value;

template<typename T>
std::enable_if_t<!hasSwap_v<T>, void> Swap(T& first, T& second)
{
    std::cout << typeid(T).name() << " has no swap\n";

    auto tmp = std::move(first);
    first = std::move(second);
    second = std::move(tmp);
}

template<typename T>
std::enable_if_t<hasSwap_v<T>, void> Swap(T& first, T& second)
{
    std::cout << typeid(T).name() << " has swap\n";

    first.swap(second);
}

int main()
{
    int one = 1;
    int two = 2;
    Swap(one, two);

    auto v1 = std::vector<int>{1, 2, 3}; 
    auto v2 = std::vector<int>{4, 5, 6}; 
    Swap(v1, v2);

    return 0;
}
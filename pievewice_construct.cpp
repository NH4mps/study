#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vcruntime.h>

struct PiecewiseConstruct {};

template<class Type, class Tuple, size_t ... Ints>
Type Make(
    std::integer_sequence<std::size_t, Ints...>,
    Tuple&& aFirstTuple)
{
    return Type(std::get<Ints>(std::move(aFirstTuple))...);
}

template <class First, class Second>
struct MyPair
{
    template <class FirstT, class SecondT>
    MyPair(FirstT&& aFirst, SecondT&& aSecond) 
    : first(std::forward<FirstT>(aFirst))
    , second(std::forward<SecondT>(aSecond)) {}

    template<class FirstTuple, class SecondTuple>
    MyPair(
        PiecewiseConstruct, 
        FirstTuple&& aFirstTuple, 
        SecondTuple&& aSecondTuple) 
    : first(Make<First>(
        std::make_index_sequence<std::tuple_size_v<std::decay_t<FirstTuple>>>{},
        std::forward<FirstTuple>(aFirstTuple)))
    , second(Make<Second>(
        std::make_index_sequence<std::tuple_size_v<std::decay_t<SecondTuple>>>{},
        std::forward<SecondTuple>(aSecondTuple))) {}

    First first;
    Second second;
};

namespace Sub 
{

struct Type1
{
    Type1() {};
    Type1(const Type1&) { std::cout << "SubType1 copy\n"; }
    Type1(Type1&&) { std::cout << "SubType1 move\n"; }
};

struct Type2
{
    Type2() {};
    Type2(const Type2&) { std::cout << "SubType2 copy\n"; }
    Type2(Type2&&) { std::cout << "SubType2 move\n"; }
};

struct Type3
{
    Type3() {};
    Type3(const Type3&) { std::cout << "SubType3 copy\n"; }
    Type3(Type3&&) { std::cout << "SubType3 move\n"; }
};

struct Type4
{
    Type4() {};
    Type4(const Type4&) { std::cout << "SubType4 copy\n"; }
    Type4(Type4&&) { std::cout << "SubType4 move\n"; }
};

struct Type5
{
    Type5() {};
    Type5(const Type5&) { std::cout << "SubType5 copy\n"; }
    Type5(Type5&&) { std::cout << "SubType5 move\n"; }
};

}

struct Type1
{
    Type1(Sub::Type1, Sub::Type2) {};
    Type1(const Type1&) { std::cout << "Type1 copy\n"; }
    Type1(Type1&&) { std::cout << "Type1 move\n"; }
};


struct Type2
{
    Type2(Sub::Type3, Sub::Type4, Sub::Type5) {};
    Type2(const Type2&) { std::cout << "Type2 copy\n"; }
    Type2(Type2&&) { std::cout << "Type2 move\n"; }
};


int main()
{
    std::cout << "============= Should move first and copy second =============\n";
    Sub::Type2 st2;
    MyPair<Sub::Type1, Sub::Type2> p1(Sub::Type1(), st2);

    std::cout << "============= Should copy first and move second =============\n";
    Type1 t1((Sub::Type1()), Sub::Type2());
    MyPair<Type1, Type2> p2(t1, Type2((Sub::Type3()), Sub::Type4(), Sub::Type5()));

    std::cout << "============= No move and no copy (subtypes moved) =============\n";
    MyPair<Type1, Type2> p3(
        PiecewiseConstruct{},
        std::forward_as_tuple(Sub::Type1(), Sub::Type2()), 
        std::forward_as_tuple(Sub::Type3(), Sub::Type4(), Sub::Type5()));

    std::cout << "============= No move and no copy (subtypes moved and copied) =============\n";
    Sub::Type4 st4;
    MyPair<Type1, Type2> p4(
        PiecewiseConstruct{},
        std::forward_as_tuple(Sub::Type1(), st2), 
        std::forward_as_tuple(Sub::Type3(), st4, Sub::Type5()));
    
    std::cout << "============= Copy first (subtypes moved and copied) =============\n";
    MyPair<Type1, Type2> p5(
        PiecewiseConstruct{},
        std::forward_as_tuple(t1), 
        std::forward_as_tuple(Sub::Type3(), st4, Sub::Type5()));
}
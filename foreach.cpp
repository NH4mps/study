#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

template<class Container>
struct Iter
{
    using Type = typename Container::iterator;

    static auto begin(Container& self)
    {
        return self.begin();
    }

    static auto end(Container& self)
    {
        return self.end();
    }
};

template<class Container>
struct Iter<const Container>
{
    using Type = typename Container::const_iterator;

    static auto begin(const Container& self)
    {
        return self.cbegin();
    }

    static auto end(const Container& self)
    {
        return self.cend();
    }
};

template <class Container>
struct ForeachWrapper
{
    using Iter = Iter<typename std::remove_reference<Container>::type>;

    ForeachWrapper(Container&& aContainer) 
        : container(std::forward<Container>(aContainer)),  it(Iter::begin(container)) {}

    void Increase()
    {
        ++it;
        control = 1;
    }

    bool IsEnd()
    {
        return it == Iter::end(container);
    }

    typename Iter::Type CurrentIt()
    {
        return it;
    }

    int control{1};

private:
    Container container;
    typename Iter::Type it;
};

template<class Container>
ForeachWrapper<Container> MakeWrapper(Container&& aContainer)
{
    return ForeachWrapper<Container>(std::forward<Container>(aContainer));
}

#define foreach(TypedVar, Container) \
    for (auto wrapper = MakeWrapper(Container); !wrapper.IsEnd(); wrapper.Increase()) \
        for (TypedVar = *wrapper.CurrentIt(); wrapper.control; wrapper.control ^= 1)


class DebugVector : public std::vector<int>
{
public:
    using Base = std::vector<int>;
    using Base::Base;

    DebugVector(const DebugVector& aOther)
    : Base(aOther)
    {
        std::cout << "Copy" << std::endl;
    }

    DebugVector(DebugVector&& aOther)
    : Base(std::move(aOther))
    {
        std::cout << "Move" << std::endl;
    }
};

int main()
{
    // lvalue
    {
        DebugVector vec = {0, 1, 5};

        std::cout << '\n';
        foreach(int elem, vec)
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(const int elem, vec)
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(int& elem, vec)
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(const int& elem, vec)
        {
            std::cout << elem << '\t';
        }
    } 

    std::cout << '\n';

    // rvalue
    {
        std::cout << '\n';
        foreach(int elem, DebugVector(5, 3))
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(const int elem, DebugVector(5, 3))
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(int& elem, DebugVector(5, 3))
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(const int& elem, DebugVector(5, 3))
        {
            std::cout << elem << '\t';
        }
    }
    
    std::cout << '\n';
    
    // const lvalue
    {
        const DebugVector vec = {0, 1, 5};

        std::cout << '\n';
        foreach(int elem, vec)
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(const int elem, vec)
        {
            std::cout << elem << '\t';
        }
    
        std::cout << '\n';
        foreach(const int& elem, vec)
        {
            std::cout << elem << '\t';
        }
    } 

}
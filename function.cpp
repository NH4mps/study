#include <iostream>
#include <memory>
#include <utility>

template<typename Ret, typename ... Args>
struct CallableBase
{
    virtual Ret operator()(Args... args) = 0;
    virtual ~CallableBase() {};
};

template<typename Functor, typename Ret, typename ... Args>
struct CallableImpl : public CallableBase<Ret, Args...>
{
    CallableImpl(Functor&& aFunctor) : mImpl(std::forward<Functor>(aFunctor)) {}

    Ret operator()(Args... aArgs) override
    {
        return mImpl(aArgs...);
    }

    Functor mImpl;
};

template<typename T>
class Function;

template<typename Ret, typename ... Args>
class Function<Ret(Args...)>
{
public:
    Function(){}

    template<typename Functor>
    Function(Functor&& aFunctor)
    {
        mImpl = std::make_unique<CallableImpl<Functor, Ret, Args...>>(std::forward<Functor>(aFunctor));
    }

    Function(const Function& aOther) = default;

    Function(Function&& aOther) = default;

    Function& operator=(const Function& aOther) = default;

    Function& operator=(Function&& aOther) = default;

    Ret operator()(Args... aArgs)
    {
        return (*mImpl)(aArgs...);
    }


private:
    std::unique_ptr<CallableBase<Ret, Args...>> mImpl;
};

int func(char c) 
{ 
    std::cout << "It is function: " << c << std::endl;
    return int(c); 
}

int func2(char c) 
{ 
    std::cout << "It is function 2: " << c << std::endl;
    return int(c); 
}

int main()
{
    Function<int(char)> f(func);
    f('G');

    Function<int(char)> f2([](char c){ std::cout << "It is lambda: " << c << std::endl; return 0; });
    f2('g');

    auto f3 = f;
    f3('I');
    
    auto f4 = func2;
    f4('i');

    f2 = f4;
    f2('H');

    f2 = [](char c) { std::cout << "It is lambda 2: " << c << std::endl; return 0; };
    f2('h');

    return 0;
}
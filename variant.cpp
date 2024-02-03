#include <functional>
#include <iostream>
#include <stdint.h>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vcruntime.h>

template <class T, class Tuple>
struct Index;

template <class T, class... Types>
struct Index<T, std::tuple<T, Types...>> {
    static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct Index<T, std::tuple<U, Types...>> {
    static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
};

template <class... Rest>
struct TypeList;

template <class First, class... Rest>
struct TypeList<First, Rest...>
{
    using Head = First;
    using Next = TypeList<Rest...>;
};

template <class Last>
struct TypeList<Last>
{
    using Head = Last;
};

template <class TL>
struct MaxSize;

template <class First, class... Rest>
struct MaxSize<TypeList<First, Rest...>>
{
    static constexpr size_t value = MaxSize<TypeList<Rest...>>::value > sizeof(First) 
        ? MaxSize<TypeList<Rest...>>::value 
        : sizeof(First);
};

template <class Last>
struct MaxSize<TypeList<Last>>
{
    static constexpr size_t value = sizeof(Last);
};

template <class Type, class TL>
struct Find;

template <class Type, class First, class... Rest>
struct Find<Type, TypeList<First, Rest...>>
{
    static constexpr bool value = std::is_same<Type, First>::value
        ? true
        : Find<Type, TypeList<Rest...>>::value;
};

template <class Type, class Last>
struct Find<Type, TypeList<Last>>
{
    static constexpr size_t value = std::is_same<Type, Last>::value;
};

template <typename ... Types>
class Variant
{
public:
    Variant()
    : mCurrentTypeIndex(0)
    {
        new (mData) typename TypeList<Types...>::Head();
    }
    
    template <typename Type>
    Variant(Type&& aObj)
    {
        using RealType = typename std::remove_reference<Type>::type;

        static_assert(Find<RealType, TypeList<Types...>>::value, "No such type in a variant");

        new (mData) RealType(std::forward<Type>(aObj));
        mCurrentTypeIndex = Index<RealType, std::tuple<Types...>>::value;
    }

    Variant(const Variant<Types...>& aOther)
    {
        mCurrentTypeIndex = aOther.mCurrentTypeIndex;
        createTable[mCurrentTypeIndex](mData, aOther.mData);
    }

    Variant(Variant<Types...>&& aOther)
    {
        mCurrentTypeIndex = aOther.mCurrentTypeIndex;
        moveTable[mCurrentTypeIndex](mData, aOther.mData);
    }

    Variant<Types...>& operator=(const Variant<Types...>& aOther) = delete;
    Variant<Types...>& operator=(Variant<Types...>&& aOther) = delete;

    template <typename Type>
    void Set(Type&& aObj)
    {
        using RealType = typename std::remove_reference<Type>::type;

        static_assert(Find<RealType, TypeList<Types...>>::value, "No such type in a variant");

        if (mCurrentTypeIndex >= 0)
        {
            destroyTable[mCurrentTypeIndex](mData);
            mCurrentTypeIndex = -1;
        }

        // if exception is thrown below, then  variant is invalidated (as in std)
        
        new (mData) RealType(std::forward<Type>(aObj)); 
        mCurrentTypeIndex = Index<RealType, std::tuple<Types...>>::value;
    }

    template<typename F>
    using Retval = typename std::result_of_t<F(typename TypeList<Types...>::Head&)>;

    template<typename Functor>
    Retval<Functor> Visit(Functor& aFunctor)
    {
        constexpr TRetrive<Functor> table[typeCount]{Retrieve<Functor, Types>...};

        return table[mCurrentTypeIndex](mData, aFunctor);
    }

    ~Variant()
    {
        if (mCurrentTypeIndex >= 0)
        {
            destroyTable[mCurrentTypeIndex](mData);
        }
    }

private:
    typedef void(*TCreate)(void*,const void*);
    
    template <typename T>
    static void Create(void* aTarget, const void* aSource)
    {
        new (aTarget) T(*(reinterpret_cast<const T*>(aSource))); 
    }

    typedef void(*TMove)(void*, void*);
    
    template <typename T>
    static void Move(void* aTarget, void* aSource)
    {
        new (aTarget) T(std::move(*(reinterpret_cast<T*>(aSource)))); 
    }

    typedef void(*TDestroy)(void*);
    
    template <typename T>
    static void Destroy(void* aObjPtr)
    {
        reinterpret_cast<T*>(aObjPtr)->~T();
    }

    template<typename Functor>
    using TRetrive = Retval<Functor>(*)(void*, Functor&);
        
    template <typename Functor, typename T>
    static Retval<Functor> Retrieve(void* aObjPtr, Functor& f)
    {
        return f(*(reinterpret_cast<T*>(aObjPtr)));
    }

private:
    static constexpr size_t bufSize = MaxSize<TypeList<Types...>>::value; 
    static constexpr size_t typeCount = sizeof...(Types);

private:
    static constexpr TDestroy destroyTable[typeCount]{Destroy<Types>...};
    static constexpr TCreate createTable[typeCount]{Create<Types>...};
    static constexpr TMove moveTable[typeCount]{Move<Types>...};

private:
    int mCurrentTypeIndex{-1};
    char mData[bufSize];
};

class Buffer
{
public:
    Buffer(size_t aSize)
    : mSize(aSize)
    , mId(1)
    {
        mData = new char[mSize];

        for (size_t i = 0; i < mSize - 1; i++)
        {
            mData[i] = 'x'; 
        }
        
        mData[mSize - 1] = '\0';
    }

    Buffer(const Buffer& aOther)
    {
        std::cout << "Another copy!\n";

        mSize = aOther.mSize;
        mId = aOther.mId + 1;

        mData = new char[mSize];

        for (size_t i = 0; i < mSize; i++)
        {
            mData[i] = aOther.mData[i]; 
        }
    }

    Buffer(Buffer&& aOther)
    {
        std::cout << "Another move!\n";

        mSize = aOther.mSize;
        mId = aOther.mId;
        mData = aOther.mData;

        aOther.mSize = 0;
        aOther.mId = 0;
        aOther.mData = nullptr;
    }

    ~Buffer()
    {
        std::cout << "Another destruction!\n";
        delete[] mData;
    }

    char* mData;
    size_t mSize;
    size_t mId;
};

std::ostream& operator<<(std::ostream& aOutput, const Buffer& aBuf)
{
    return aOutput << "{" << aBuf.mData << ", " << aBuf.mSize << "}";
}

auto f(size_t i)
{
    return Buffer(i);
}

auto f2(size_t i)
{
    auto functor = f;

    return functor(i);
}

class Jopa
{
public:
    Jopa(const Buffer& aBuffer) : ref(aBuffer)
    {

    }

private:
    const Buffer& ref;
};

int main()
{
    using Var = Variant<int, std::string, Buffer>;

    Var var(5);

    auto lambda = [](auto& aObj) -> Var { std::cout << aObj << std::endl; return aObj; };

    var.Set(7);

    var.Visit(lambda);

    var.Set(Buffer(7));

    std::cout << "-----Copies after visit:\n";

    var.Visit(lambda);
}
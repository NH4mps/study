#include <iostream>
#include <vector>

class NulloptType 
{};

constexpr NulloptType Nullopt{};

template<class T>
class Optional
{
public:
    Optional()
    : mInited(false)
    {}

    Optional(const NulloptType& aValue)
    : mInited(false)
    {}

    template<class V>
    Optional(V&& aValue)
    : mInited(true)
    {
        new (mData) T(std::forward<V>(aValue));
    }

    ~Optional()
    {
        if (mInited)
        {
            reinterpret_cast<T*>(mData)->~T();
        }
    }

    T& operator*()
    {
        return *(reinterpret_cast<T*>(mData));
    }

    T& operator->()
    {
        return **this;
    }

    const T& operator*() const
    {
        return *(reinterpret_cast<T*>(mData));
    }

    const T& operator->() const
    {
        return **this;
    }

    bool HasValue() const
    {
        return mInited;
    }

    bool operator==(const Optional<T>& aObj)
    {
        if (aObj.HasValue() && this->HasValue())
        {
            return *aObj == **this;
        }
        else if (!aObj.HasValue() && !this->HasValue())
        {
            return true;
        }

        return false;
    }

    bool operator=(const Optional<T>& aObj)
    {
        this->~Optional<T>();

        if (aObj.mInited)
        {
            new (mData) T(aObj.mData);
        }

        mInited = aObj.mInited;
    }

private:
    bool mInited;
    char mData[sizeof(T)];
};

struct Type
{
    Type(){};
    Type(const Type&) { std::cout << "copy\n"; }
    Type(Type&&) { std::cout << "move\n"; }
    ~Type() { std::cout << "destructed\n"; }
};


int main()
{
    Type typeVar1;
    Optional<Type> var1{Type()};
    Optional<Type> var2{typeVar1};
}
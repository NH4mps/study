#include <assert.h>

#define GET_TRUE_TYPE_REALIZATION

template <typename First, typename ... Others>
struct Tuple
{
   Tuple(const First& v, const Others& ... n) : value(v), next(n...) {}

   First value;
   Tuple<Others...> next;
};

template<typename Last>
struct Tuple<Last>
{
   Tuple(const Last& v) : value(v) {}

   Last value;
};

template <typename First, typename ... Others>
Tuple<First, Others...> makeTuple(First&& v, Others&& ... n)
{
   return Tuple<First, Others...>(v, n...);
}

//////////////////////////////////////////////////////////

template <size_t Num, typename First, typename ... Others>
struct TupleElement
{
   using Type = typename TupleElement<Num - 1, Others...>::Type;
};

template <typename First, typename ... Others>
struct TupleElement<0, First, Others...>
{
   using Type = First;
};

//////////////////////////////////////////////////////////

template <typename First, typename ... Others>
struct Tie
{
   Tie& operator=(const Tuple<First, Others...>& t)
   {
      value = t.value;
      next = t.next;

      return *this;
   }

   Tie(First& v, Others& ... o) : value(v), next(o...) {}

   First& value;
   Tie<Others...> next;
};

template <typename Last>
struct Tie<Last>
{
   Tie& operator=(const Tuple<Last>& t)
   {
      value = t.value;

      return *this;
   }

   Tie(Last& v) : value(v) {}

   Last& value;
};

template <typename First, typename ... Others>
Tie<First, Others...> makeTie(First& v, Others& ... n)
{
   return Tie<First, Others...>(v, n...);
}

#ifdef GET_TRUE_TYPE_REALIZATION

struct TrueType{};
struct FalseType{};

template <size_t Num>
struct IsNumZeroType
{
   using ResType = FalseType;
};

template<>
struct IsNumZeroType<0>
{
   using ResType = TrueType;
};

template<size_t Num, typename First, typename ... Others>
constexpr typename TupleElement<Num, First, Others... >::Type& 
get(Tuple<First, Others...>& t)
{
   return get_impl<Num>(IsNumZeroType<Num>::ResType(), t);
}

template<size_t Num, typename First, typename ... Others>
constexpr typename TupleElement<Num, First, Others... >::Type& 
get_impl(FalseType, Tuple<First, Others...>& t)
{
   return get<Num - 1, Others...>(t.next);
}

template<size_t Num, typename First, typename ... Others>
constexpr typename TupleElement<Num, First, Others... >::Type& 
get_impl(TrueType, Tuple<First, Others...>& t)
{
   return t.value;
}

#else // ENABLE IF REALIZATION
// TODO
#endif // GET REALIZATIONS


int main()
{
   Tuple<int, double> tup(2, 4.5);
   assert(tup.value == 2 && tup.next.value == 4.5);

   TupleElement<0, int, double>::Type i = 1;
   TupleElement<1, int, double>::Type d = 2.0;

   i = get<0>(tup);
   d = get<1>(tup);
   assert(i == tup.value && d == tup.next.value);

   int iToBeTied = 0;
   double dToBeTied = 0.0;

   Tie<int, double>(iToBeTied, dToBeTied) = tup;
   assert(iToBeTied == tup.value && dToBeTied == tup.next.value);

   auto tup2 = makeTuple(3, 2.28);
   assert(tup2.value == 3 && tup2.next.value == 2.28);
 
   int iToBeTied2 = 0;
   double dToBeTied2 = 0.0;

   makeTie(iToBeTied2, dToBeTied2) = tup2;
   assert(iToBeTied2 == tup2.value && dToBeTied2 == tup2.next.value);

   return 0;
}
#pragma once



namespace talk_base
{
namespace detail
{

template <class U> struct ReferenceTraits
{
    enum { result = false };
    typedef U ReferredType;
};

template <class U> struct ReferenceTraits<U&>
{
    enum { result = true };
    typedef U ReferredType;
};

template <class U> struct UnConst
{
    typedef U Result;
    enum { isConst = 0 };
};

template <class U> struct UnConst<const U>
{
    typedef U Result;
    enum { isConst = 1 };
};

template <class U> struct ValueType
{
	typedef typename UnConst< typename ReferenceTraits<U>::ReferredType >::Result Result;
};

struct NullType;

struct EmptyType{};

template <class U> struct PointerTraits
{
    enum { result = false };
    typedef void* PointeeType;
};
        
template <class U> struct PointerTraits<U*>
{
    enum { result = true };
    typedef U PointeeType;
};
        
template <class U> struct PointerTraits<U*&>
{
    enum { result = true };
    typedef U PointeeType;
};

template <class U> struct UnVolatile
{
    typedef U Result;
    enum { isVolatile = 0 };
};

template <class U> struct UnVolatile<volatile U>
{
    typedef U Result;
    enum { isVolatile = 1 };
};

template < typename T > struct VolatileType
{
	typedef typename volatile UnVolatile<T>::Result Result;
};

template <typename T> struct UnvolatileType
{
	typedef typename UnVolatile<T>::Result Result;
};

//parameter type begin
template <class U> struct ParameterType
{
	typedef typename const ValueType<U>::Result& Result;
};
template <class U> struct ParameterType<U*>
{
	typedef typename ValueType<U>::Result* Result;
};
template <class U> struct ParameterType<const U*>
{
	typedef const typename ValueType<U>::Result* Result;
};
template <class U> struct ParameterType<U*&>
{
	typedef typename ValueType<U>::Result* Result;
};
template <class U> struct ParameterType<const U*&>
{
	typedef typename const ValueType<U>::Result* Result;
};

template <> struct ParameterType<char>
{
	typedef char Result;
};
template <> struct ParameterType<unsigned char>
{
	typedef unsigned char Result;
};
template <> struct ParameterType<short>
{
	typedef short Result;
};
template <> struct ParameterType<unsigned short>
{
	typedef unsigned short Result;
};
template <> struct ParameterType<int>
{
	typedef int Result;
};
template <> struct ParameterType<unsigned int>
{
	typedef unsigned int Result;
};
template <> struct ParameterType<long>
{
	typedef long Result;
};
template <> struct ParameterType<unsigned long>
{
	typedef unsigned long Result;
};
#ifdef WIN32
template <> struct ParameterType<__int64>
{
	typedef __int64 Result;
};
template <> struct ParameterType<unsigned __int64>
{
	typedef unsigned __int64 Result;
};
#endif
template <> struct ParameterType<float>
{
	typedef float Result;
};
template <> struct ParameterType<double>
{
	typedef double Result;
};
//parameter type end


template<int> struct CompileTimeError;
template<> struct CompileTimeError<true> {};

}

}


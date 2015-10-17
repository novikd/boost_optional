//
// Created by 1 on 17.10.2015.
//
#include <tuple>

#ifndef BIND_BIND_H
#define BIND_BIND_H

#endif //BIND_BIND_H

template <size_t N>
struct arg;

template <>
struct arg<0>
{
    template <typename First, typename ... Tail>
    typename std::decay<First const>::type operator() (First const& first, Tail const& ... tail) const
    {
        return first;
    }
};

template <size_t N>
struct arg
{
    template <typename First, typename ... Tail>
    auto operator() (First const& first, Tail const& ... tail) const -> decltype(arg<N - 1>()(tail...))
    {
        return arg<N - 1>()(tail...);
    }
};

const arg<0> _1{};
const arg<1> _2{};
const arg<2> _3{};
const arg<3> _4{};
const arg<4> _5{};
const arg<5> _6{};
const arg<6> _7{};

template <typename T>
struct const_
{
    const_ (T val)
            :data(val)
    {}

    template <typename ... Args>
    T operator() (Args ... args) const
    {
        return data;
    }

private:
    T data;
};

template <typename P>
struct holder
{
    typedef const_<P> type;
};

template <size_t N>
struct holder<arg<N>>
{
    typedef arg<N> type;
};

template <size_t ... Indeces>
struct index_list
{
    typedef index_list<Indeces..., sizeof...(Indeces)> next;
};

template <size_t N>
struct indeces_by_n
{
    typedef typename  indeces_by_n<N - 1>::type::next type;
};

template <>
struct indeces_by_n<0>
{
    typedef index_list<> type;
};

template <typename F, typename ...P>
struct bind_t
{
private:
    F func;
    std::tuple<typename :: holder<P>::type...> p;

public:
    bind_t(F f, P ... pr)
        :func(std::move(f)),
         p(std::move(pr)...)
    {}

    template <size_t ... N, typename  ... Args>
    auto call(index_list<N...>, Args const& ... args) const -> decltype(func(std::get<N>(p)(args...)...))
    {
        return func(std::get<N>(p)(args...)...);
    }

    template <typename ... Args>
    auto operator() (Args const& ... args) const -> decltype(this->call(typename indeces_by_n<sizeof...(P)>::type(), args...))
    {
        typename indeces_by_n<sizeof...(P)>::type indices;
        return call(indices, args...);
    }

};

template <typename F, typename ... P>
struct holder<bind_t<F, P ...>>
{
    typedef bind_t<F, P ...> type;
};

template <typename F, typename ... P>
auto bind(F f, P ... p) -> decltype(bind_t<F, P ...>(f, p ...))
{
    return bind_t<F, P ...>(f, p ...);
};

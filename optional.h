//
// Created by 1 on 03.10.2015.
//

#ifndef BOOST_OPTIONAL_OPTIONAL_H
#define BOOST_OPTIONAL_OPTIONAL_H

#endif //BOOST_OPTIONAL_OPTIONAL_H

#include <assert.h>
#include "type_traits"

struct none_t {} none;

template <typename T>
struct optional {

    optional():
            is_none(true)
    {}

    optional(optional<T> const& other):
            is_none(other.is_none)
    {
        if (!is_none)
            new (&storage) T(*other);
    }

    optional(T const& data):
            is_none(false)
    {
        new (&storage) T(data);
    }

    optional& operator=(T const& data)
    {
        if (!is_none) delete_data();
        new (&storage) T(data);
        is_none = false;
        return *this;
    }

    optional& operator=(optional<T> const& other)
    {
        if (!is_none)
        {
            delete_data();
            create_storage(other.storage);
        }
        is_none = other.is_none;
        return *this;
    }

    optional& operator=(none_t none)
    {
        if (!is_none) delete_data();
        return *this;
    }

    T& operator*()
    {
        assert(!is_none);
        return *reinterpret_cast<T*>(&storage);
    }

    T const& operator*() const
    {
        assert(!is_none);
        return *reinterpret_cast<const T*>(&storage);
    }

    T* operator->()
    {
        assert(!is_none);
        return reinterpret_cast<T*>(&storage);
    }

    T const* operator->() const
    {
        assert(!is_none);
        return reinterpret_cast<T const*>(&storage);
    }

    explicit operator bool() const
    {
        return !is_none;
    }

    ~optional()
    {
        if (!is_none)
            this->delete_data();
    }

    template <typename ... Args>
    optional<T>& emplace(Args ... args)
    {
        if (!is_none)
            delete_data();
        new (&storage) T(args...);
        is_none = false;
        return *this;
    }

    friend bool operator==(optional<T> const& frs, optional<T> const& snd) {
        return (frs.is_none && snd.is_none) || (!frs.is_none && !snd.is_none &&
                                                *frs == *snd);
    }

    friend bool operator!=(optional<T> const& frs, optional<T> const& snd)
    {
        return !(frs == snd);
    }

    friend bool operator<(optional<T> const& frs, optional<T> const& snd)
    {
        if (frs == snd) return false;
        return frs.is_none || *frs < *snd;
    }

    friend bool operator>(optional<T> const& frs, optional<T> const& snd)
    {
        return snd < frs;
    }

    friend bool operator<=(optional<T> const& frs, optional<T> const& snd)
    {
        return (frs < snd) || (frs == snd);
    }

    friend bool operator>=(optional<T> const& frs, optional<T> const& snd)
    {
        return (frs > snd) || (frs == snd);
    }

    void swap(optional<T>& other) {
        //if (*this == other) return;
        if (is_none && other.is_none) return;
        if (is_none) {
            create_storage(other.storage);
            other.delete_data();
            is_none = false;
            return;
        }
        if (other.is_none) {
            other.create_storage(storage);
            other.is_none = false;
            delete_data();
            return;
        }
        std::swap(storage, other.storage);
    }

private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
    bool is_none;
    void create_storage(typename std::aligned_storage<sizeof(T), alignof(T)>::type const& other)
    {
        new (&storage) T(*reinterpret_cast<T const*>(&other));
    }
    void delete_data() {
        is_none = true;
        reinterpret_cast<T&>(storage).~T();
    }
};

template <typename S, typename ... Args>
optional<S> make_optional(Args ... args) {
    optional<S> tmp;
    tmp.emplace(args...);
    return tmp;
}

namespace std {
    template <typename T>
    void swap(optional<T>& a, optional<T>& b)
    {
        a.swap(b);
    }
}

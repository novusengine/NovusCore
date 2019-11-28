#pragma once
#include <type_safe/strong_typedef.hpp>
#include <limits>

template <class StrongTypedef>
struct TextureComparison
{
    friend bool operator==(StrongTypedef& lhs, const StrongTypedef& rhs)
    {
        using type = type_safe::underlying_type<StrongTypedef>;
        return static_cast<type&>(lhs) == static_cast<const type&>(rhs);
    }

    friend bool operator==(const StrongTypedef& lhs, const StrongTypedef& rhs)
    {
        using type = type_safe::underlying_type<StrongTypedef>;
        return static_cast<type&>(lhs) == static_cast<const type&>(rhs);
    }

    friend bool operator!=(StrongTypedef& lhs, const StrongTypedef& rhs)
    {
        using type = type_safe::underlying_type<StrongTypedef>;
        return static_cast<type&>(lhs) != static_cast<const type&>(rhs);
    }

    friend bool operator!=(const StrongTypedef& lhs, const StrongTypedef& rhs)
    {
        using type = type_safe::underlying_type<StrongTypedef>;
        return static_cast<type&>(lhs) != static_cast<const type&>(rhs);
    }
};

// If you want to change the underlying integer type used for these handles, change the type under "HERE"
//                                                              HERE
struct TextureHandle : type_safe::strong_typedef<TextureHandle, u16>, TextureComparison<TextureHandle>
{
    using strong_typedef::strong_typedef;
    using type = type_safe::underlying_type<TextureHandle>;

    static size_t MaxValue()
    {
        return std::numeric_limits<type>::max();
    }

    static type MaxValueTyped()
    {
        return (type)std::numeric_limits<type>::max();
    }

    static TextureHandle Invalid()
    {
        return TextureHandle(TextureHandle::MaxValueTyped());
    }
};
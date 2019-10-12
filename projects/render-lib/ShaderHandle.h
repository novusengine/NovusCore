#pragma once
#include <type_safe/strong_typedef.hpp>
#include <limits>

template <class StrongTypedef>
struct comparison
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
};

// If you want to change the underlying integer type used for these handles, change the type under "HERE"
//                                                                HERE
struct __ShaderHandle : type_safe::strong_typedef<__ShaderHandle, u16>, comparison<__ShaderHandle>
{
    using strong_typedef::strong_typedef;

    static size_t MaxValue()
    {
        using type = type_safe::underlying_type<__ShaderHandle>;
        return std::numeric_limits<type>::max();
    }
};

struct ShaderHandleVert :type_safe::strong_typedef<ShaderHandleVert, __ShaderHandle>
{
    using strong_typedef::strong_typedef;
};

struct ShaderHandleFrag :type_safe::strong_typedef<ShaderHandleFrag, __ShaderHandle>
{
    using strong_typedef::strong_typedef;
};

struct ShaderHandleComp :type_safe::strong_typedef<ShaderHandleComp, __ShaderHandle>
{
    using strong_typedef::strong_typedef;
};
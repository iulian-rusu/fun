#ifndef FUN_UTILITY_HPP
#define FUN_UTILITY_HPP

#include <fun/any.hpp>

namespace fun
{
    // Mechanism used to associate an integral value with a type
    template<std::intmax_t ID>
    struct tag : std::integral_constant<std::intmax_t, ID>{};

    // Utility functions used to get rvalues or lvalues of any type (in an unevaluated context)
    template<typename T> constexpr auto rvalue() noexcept -> decltype(std::declval<T>());
    template<typename T> constexpr auto lvalue() noexcept -> decltype(std::declval<T &>());

    // Template to allow expansion with std::index_sequence
    template<std::size_t = 0> constexpr auto rvalue() noexcept -> any &&;
    template<std::size_t = 0> constexpr auto lvalue() noexcept -> any &;

    inline constexpr auto noop = []() noexcept {};
}
#endif //FUN_UTILITY_HPP
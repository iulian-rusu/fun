#ifndef FUN_UTILITY_HPP
#define FUN_UTILITY_HPP

#include <fun/any.hpp>

namespace fun
{
    inline constexpr auto noop = []() {};

    // Helper type used to map an integer sequence to a type sequence
    template<std::size_t, typename T>
    using generate_type = T;

    // Mechanism used to associate an integral value with a type
    template<std::intmax_t ID>
    struct tag : std::integral_constant<std::intmax_t, ID>{};

    // Utility functions used to get rvalues or lvalues of any type
    template<typename T> constexpr auto rvalue() noexcept -> decltype(std::declval<T>());
    template<typename T> constexpr auto lvalue() noexcept -> decltype(std::declval<T &>());

    // Template to allow expansion with std::index_sequence
    template<std::size_t = 0> constexpr auto rvalue() noexcept -> decltype(std::declval<any>());
    template<std::size_t = 0> constexpr auto lvalue() noexcept -> decltype(std::declval<any &>());
}
#endif //FUN_UTILITY_HPP
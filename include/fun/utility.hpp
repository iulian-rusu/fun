#ifndef FUN_UTILITY_HPP
#define FUN_UTILITY_HPP

#include <fun/any.hpp>

namespace fun
{
    // Type trait for using value semantics for trivially copyable types and const references otherwise.
    template<typename T>
    using arg_t =
            std::conditional_t
            <
                std::is_trivially_copyable_v<std::remove_cvref_t<T>>,
                std::remove_cvref_t<T>,
                std::remove_cvref_t<T> const &
            >;

    // Helper type used to map an integer sequence to a type sequence
    template<std::size_t, typename T>
    using map_index_t = T;

    // Mechanism used to associate an integral value with a type
    template<auto ID>
    struct tag_type : std::integral_constant<decltype(ID), ID>{};

    template<auto ID>
    inline constexpr auto tag = tag_type<ID>{};

    // Utility functions used to get rvalues or lvalues of any type
    template<typename T> constexpr auto rvalue() noexcept -> decltype(std::declval<T>());
    template<typename T> constexpr auto lvalue() noexcept -> decltype(std::declval<T &>());

    // Template to allow expansion with std::index_sequence
    template<auto = 0> constexpr auto rvalue() noexcept -> decltype(std::declval<any>());
    template<auto = 0> constexpr auto lvalue() noexcept -> decltype(std::declval<any &>());
}
#endif //FUN_UTILITY_HPP
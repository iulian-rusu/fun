#ifndef FUN_ANY_HPP
#define FUN_ANY_HPP

#include <utility>

namespace fun
{
    /**
     * Type that can be used in an unevaluated context as a placeholder for anything.
     * It is constructible and asignable from anything, as well as implicitly convertible to any type.
     */
    struct any
    {
        template<typename... T>
        constexpr any(T &&...) noexcept;

        template<typename T>
        constexpr any &operator=(T &&) noexcept;

        template<typename T>
        constexpr explicit(false) operator T() const noexcept;
    };

    // Utility functions used to get rvalues or lvalues of any type
    template<typename T> constexpr auto rvalue() noexcept -> decltype(std::declval<T>());
    template<typename T> constexpr auto lvalue() noexcept -> decltype(std::declval<T &>());

    // Template to allow expansion with std::index_sequence
    template<auto = 0> constexpr auto rvalue() noexcept -> decltype(std::declval<any>());
    template<auto = 0> constexpr auto lvalue() noexcept -> decltype(std::declval<any &>());
}
#endif //FUN_ANY_HPP
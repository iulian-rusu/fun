#ifndef FUN_ANY_HPP
#define FUN_ANY_HPP

#include <utility>

namespace fun
{
    /**
     * Type that can be used as a placeholder for anything.
     * It is constructible and asignable from anything, as well as implicitly convertible to any type.
     */
    struct any
    {
        template<typename... T>
        constexpr explicit(false) any(T &&...) noexcept {}

        template<typename T>
        constexpr any &operator=(T &&) noexcept { return *this; }

        template<typename T>
        constexpr auto operator<=>(T const &) const noexcept { return std::strong_ordering::equal; }

        template<typename T>
        constexpr bool operator==(T const &) const noexcept { return true; }

        // Requires unevaluated context
        template<typename T>
        constexpr explicit(false) operator T() const noexcept;
    };
}
#endif //FUN_ANY_HPP
#ifndef FUN_NULLSINK_HPP
#define FUN_NULLSINK_HPP

#include <functional>
#include <fun/utility.hpp>

namespace fun
{
    namespace detail
    {
        template<std::invocable F, std::size_t... Indices>
        constexpr decltype(auto) nullsink_impl(F &&f, std::index_sequence<Indices ...> &&) noexcept
        {
            return [f = std::forward<F>(f)](generate_type<Indices, any>...)
                noexcept(std::is_nothrow_invocable_v<F>) -> decltype(auto) {
                    return std::invoke(f);
                };
        }
    }

    /**
     * Creates a null sink function that consumes and discards N parameters of any type,
     * invoking a given callback afterwards.
     *
     * @tparam N    The number of parameters consumed
     * @param f     The callback function
     * @return      The sink
     */
    template<std::size_t N, std::invocable F>
    constexpr decltype(auto) nullsink(F &&f) noexcept
    {
        return detail::nullsink_impl(std::forward<F>(f), std::make_index_sequence<N>{});
    }

    template<std::size_t N>
    constexpr decltype(auto) nullsink() noexcept
    {
        return nullsink<N>(noop);
    }
}
#endif //FUN_NULLSINK_HPP
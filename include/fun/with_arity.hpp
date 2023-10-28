#ifndef FUN_WITH_ARITY_HPP
#define FUN_WITH_ARITY_HPP

#include <functional>
#include <utility>
#include <fun/utility.hpp>

namespace fun
{
    /**
     * Enforces a given arity on a received function.
     *
     * @tparam N    The exact number of parameters (arity) enforced
     * @param f     The function to be adapted
     * @return      A new function that must be called with exactly N parameters
     */
    template<std::size_t N, traits::callable F>
    [[nodiscard]] constexpr decltype(auto) with_arity(F &&f) noexcept
    {
        return [f = std::forward<F>(f)]<typename... Args> requires (sizeof... (Args) == N)(Args &&... args)
            noexcept(std::is_nothrow_invocable_v<F &&, Args &&...>) -> decltype(auto) {
                return std::invoke(f, std::forward<Args>(args) ...);
            };
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
    [[nodiscard]] constexpr decltype(auto) nullsink(F &&f) noexcept
    {
        return with_arity<N>(
            [f = std::forward<F>(f)](auto &&...) noexcept(std::is_nothrow_invocable_v<F &&>) -> decltype(auto) {
                return std::invoke(f);
            }
        );
    }

    template<std::size_t N>
    [[nodiscard]] constexpr decltype(auto) nullsink() noexcept
    {
        return nullsink<N>(noop);
    }
}
#endif //FUN_WITH_ARITY_HPP
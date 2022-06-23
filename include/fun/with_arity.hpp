#ifndef FUN_WITH_ARITY_HPP
#define FUN_WITH_ARITY_HPP

#include <tuple>
#include <functional>
#include <fun/utility.hpp>

namespace fun
{
    namespace detail
    {
        template<typename F, std::size_t... Indices>
        constexpr decltype(auto) with_arity_impl(F &&f, std::index_sequence<Indices ...> &&) noexcept
        {
            return [captured = std::tuple<F>{std::forward<F>(f)}]
                <typename... Args>(map_index_t<Indices, Args> &&... args)
                noexcept(std::is_nothrow_invocable_v<F, Args &&...>) -> decltype(auto) {
                    return std::invoke(std::get<0>(captured), std::forward<Args>(args) ...);
                };
        }
    }

    /**
     * Enforces a given arity on a received function.
     *
     * @tparam N    The exact number of parameters (arity) enforced
     * @param f     The function to be adapted
     * @return      A new function that must be called with exactly N parameters
     */
    template<std::size_t N, typename F>
    constexpr decltype(auto) with_arity(F &&f) noexcept
    {
        return detail::with_arity_impl(std::forward<F>(f), std::make_index_sequence<N>{});
    }
}
#endif //FUN_WITH_ARITY_HPP
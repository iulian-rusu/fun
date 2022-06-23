#ifndef FUN_SINK_HPP
#define FUN_SINK_HPP

#include <tuple>
#include <functional>
#include <fun/utility.hpp>

namespace fun
{
    namespace detail
    {
        template<typename F, std::size_t... Indices>
        constexpr decltype(auto) sink_impl(F &&f, std::index_sequence<Indices ...> &&) noexcept
        {
            return [captured = std::tuple<F>{std::forward<F>(f)}]
                (map_index_t<Indices, any>...) noexcept(std::is_nothrow_invocable_v<F>) -> decltype(auto) {
                    return std::invoke(std::get<0>(captured));
                };
        }
    }

    /**
     * Creates a sink function that consumes any N parameters and then calls a given callback.
     *
     * @tparam N    The number of parameters consumed
     * @param f     The callback function
     * @return      The sink
     */
    template<std::size_t N, typename F>
    constexpr decltype(auto) sink(F &&f) noexcept
    {
        return detail::sink_impl(std::forward<F>(f), std::make_index_sequence<N>{});
    }
}
#endif //FUN_SINK_HPP
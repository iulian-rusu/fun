#ifndef FUN_CURRY_HPP
#define FUN_CURRY_HPP

#include <tuple>
#include <functional>

namespace fun
{
    namespace detail
    {
        template<typename F, typename... Args>
        struct is_nothrow_if_invocable : std::true_type {};

        template<typename F, typename... Args>
        requires std::is_invocable_v<F, Args ...>
        struct is_nothrow_if_invocable<F, Args ...> : std::is_nothrow_invocable<F, Args ...> {};

        template<typename F, typename... Args>
        inline constexpr bool is_nothrow_if_invocable_v = is_nothrow_if_invocable<F, Args ...>::value;

        template<typename F, typename... Args>
        constexpr decltype(auto) curry_impl(F &&f, Args &&... args) noexcept(is_nothrow_if_invocable_v<F &&, Args &&...>)
        {
            if constexpr (requires { std::invoke(std::forward<F>(f), std::forward<Args>(args) ...); })
                return std::invoke(std::forward<F>(f), std::forward<Args>(args) ...);
            else
                return [captures = std::make_tuple(std::forward<F>(f), std::forward<Args>(args) ...)]
                    <typename Arg>(Arg &&arg) mutable noexcept -> decltype(auto) {
                        return std::apply(
                            [&]<typename... Ts>(Ts &&... ts) noexcept -> decltype(auto) {
                                return curry_impl(std::forward<Ts>(ts) ..., std::forward<Arg>(arg));
                            },
                            std::move(captures)
                        );
                    };
        }

        template<typename F, typename Arg, typename... Args>
        constexpr decltype(auto) uncurry_impl(F &&f, Arg &&arg, Args &&... args) noexcept(is_nothrow_if_invocable_v<F &&, Arg &&>)
        {
            if constexpr (sizeof... (Args) == 0)
                return std::invoke(std::forward<F>(f), std::forward<Arg>(arg));
            else
                return uncurry_impl(
                    std::invoke(std::forward<F>(f), std::forward<Arg>(arg)),
                    std::forward<Args>(args) ...
                );
        }
    }

    /**
     * Adapts a given callable object to allow curried invocation. A curried function can be
     * invoked by passing arguments one at a time.
     * Calling the curried object with invalid argument types will not generate compilation errors
     * but will never actually call the original function.
     *
     * @tparam F    The type of the callable being adapted
     * @param f     The callable object being adapted
     * @return      A lambda expression which behaves as a curried version of the original callable
     */
    template<typename F>
    [[nodiscard]] constexpr decltype(auto) curry(F &&f) noexcept
    {
        if constexpr (requires{ std::invoke(std::forward<F>(f)); })
            return std::forward<F>(f);
        else
            return detail::curry_impl(std::forward<F>(f));
    }

    /**
     * Receives a curried calalble object and returns a lambda expression which allows passing
     * multiple parameters at once to the initial curried function. Should only be called on
     * curried functors.
     *
     * @tparam F    The type of the curried callable
     * @param f     The curried callable
     * @return      A lambda expression which behaves as a non-curried version of the function
     */
    template<typename F>
    [[nodiscard]] constexpr decltype(auto) uncurry(F &&f) noexcept
    {
        if constexpr (requires{ std::invoke(std::forward<F>(f)); })
            return std::forward<F>(f);
        else
            return [captured = std::make_tuple(std::forward<F>(f))]
                <typename Arg, typename... Args>(Arg &&arg, Args &&... args)
                mutable noexcept(std::is_nothrow_invocable_v<F &&, Arg &&>) -> decltype(auto) {
                    return detail::uncurry_impl(
                        std::invoke(std::forward<F>(std::get<0>(captured)), std::forward<Arg>(arg)),
                        std::forward<Args>(args) ...
                    );
                };
    }
}
#endif //FUN_CURRY_HPP
#ifndef FUNCTIONAL_CURRY_HPP
#define FUNCTIONAL_CURRY_HPP

#include <functional>
#include <tuple>

namespace fun
{
    namespace
    {
        template<typename F, typename... Args>
        struct is_nothrow_if_invocable : std::true_type {};

        template<typename F, typename... Args>
        requires std::is_invocable_v<F, Args ...>
        struct is_nothrow_if_invocable<F, Args ...> : std::is_nothrow_invocable<F, Args ...> {};

        template<typename F, typename... Args>
        inline constexpr bool is_nothrow_if_invocable_v = is_nothrow_if_invocable<F, Args ...>::value;

        template<typename F, typename... Args>
        constexpr decltype(auto) curry_impl(F &&f, Args &&... args) noexcept(is_nothrow_if_invocable_v<F, Args ...>)
        {
            if constexpr (requires { std::invoke(f, std::forward<Args>(args) ...); })
                return std::invoke(std::forward<F>(f), std::forward<Args>(args) ...);
            else
                return [captures = std::make_tuple(std::forward<F>(f), std::forward<Args>(args) ...)]
                    <typename... Xs>(Xs &&... xs) mutable noexcept -> decltype(auto) {
                        return std::apply(
                            [&]<typename... Ts>(Ts &&... ts) noexcept -> decltype(auto) {
                                return curry_impl(std::forward<Ts>(ts) ..., std::forward<Xs>(xs) ...);
                            },
                            captures
                        );
                    };
        }
    }

    /**
     * Adapts a given callable object to allow curried invocation. A curried function can be
     * invoked by passing arguments one at a time.
     *
     * @tparam F    The type of the callable being adapted
     * @param f     The callable object being adapted
     * @return      A lambda expression which behaves as a curried version of the original callable
     */
    template<typename F>
    constexpr decltype(auto) curry(F &&f) noexcept
    {
        // If the function does not take any arguments, forward it back to the caller
        if constexpr (requires{ std::invoke(f); })
            return std::forward<F>(f);
        else
            return curry_impl(std::forward<F>(f));
    }
}
#endif //FUNCTIONAL_CURRY_HPP
#ifndef FUNCTIONAL_CURRY_HPP
#define FUNCTIONAL_CURRY_HPP

#include <functional>

/**
 * Type trait that finds if the callable F may throw when invoked
 * with the given arguments. If F cannot be invoked, it is considered
 * noexcept by default.
 *
 * @tparam F    The callable type being tested.
 * @tparam Args The arguments for the callable.
 */
template<typename F, typename... Args>
struct is_nothrow_if_invocable : std::true_type {};

template<typename F, typename... Args>
requires std::is_invocable_v<F, Args ...>
struct is_nothrow_if_invocable<F, Args ...> : std::is_nothrow_invocable<F, Args ...> {};

template<typename F, typename... Args>
inline constexpr bool is_nothrow_if_invocable_v = is_nothrow_if_invocable<F, Args ...>::value;

/**
 * Adapts a given callable object to allow curried invocation. A curried function can be
 * invoked by passing arguments one at a time.
 *
 * @tparam F    The type of the callable being adapted.
 * @tparam Args The type of arguments for the callable.
 * @param f     The callable object being adapted.
 * @param args  The values for the arguments to the callable.
 * @return      A lambda expression which behaves as a curried version of the original callable.
 */
template<typename F, typename... Args>
decltype(auto) curry(F &&f, Args &&... args) noexcept(is_nothrow_if_invocable_v<F, Args ...>)
{
    using nothrow_t = is_nothrow_if_invocable<F, Args ...>;

    if constexpr (requires { std::invoke(f, std::forward<Args>(args) ...); })
        return std::invoke(f, std::forward<Args>(args) ...);
    else
        return [&f, ...args = std::forward<Args>(args)](auto &&... xs) mutable noexcept(nothrow_t{}) {
            return curry(f, std::forward<Args>(args) ..., std::forward<decltype(xs)>(xs) ...);
        };
}

#endif //FUNCTIONAL_CURRY_HPP

#ifndef FUN_OVERLOAD_HPP
#define FUN_OVERLOAD_HPP

#include <variant>
#include <fun/function.hpp>
#include <fun/traits.hpp>

namespace fun
{
    namespace detail
    {
        template<traits::callable F>
        struct callable_wrapper_impl : std::type_identity<F> {};

        template<traits::callable F>
        requires (!std::is_class_v<F>)
        struct callable_wrapper_impl<F> : std::type_identity<decltype(function{std::declval<F>()})> {};

        template<traits::callable F>
        using callable_wrapper = typename detail::callable_wrapper_impl<F>::type;
    }

    /**
     * A class that models the overload set of a series of callable objects.
     * All callable objects must have a different signature for their operator().
     * The result of calling operator() is determined using operator overloading rules.
     *
     * @tparam Fs   The types of the callable objects that determine this functor's behaviour
     */
    template<traits::callable... Fs>
    struct overload_set : Fs ...
    {
        using Fs::operator() ...;
    };

    template<traits::callable... Fs>
    overload_set(Fs &&...) -> overload_set<detail::callable_wrapper<std::remove_reference_t<Fs>> ...>;

    /**
     * Creates an overload set that contains all the given function signatures at once.
     *
     * @tparam Fs   The types of the callable objects that will be combined into one overload set
     * @param fs    The callable objects
     * @return  The overload set instance
     */
    template<traits::callable... Fs>
    [[nodiscard]] constexpr auto overload(Fs &&... fs) noexcept(noexcept(overload_set{std::forward<Fs>(fs) ...}))
    {
        return overload_set{std::forward<Fs>(fs) ...};
    }

    /**
     * Matches a given variant object agains a list of possible function signatures.
     * The matching functions must all return the same type when invoked with any alternative of the variant.
     *
     * @tparam Variant  The type of the variant object that will be matched
     * @tparam Matchers The types of the functions that will be used for matching
     * @param variant   The variant object beign matched
     * @param matchers  The functions used to match the object
     * @return  The result of matching the object agains the functions.
     */
    template<typename Variant, typename... Matchers>
    [[nodiscard]] constexpr auto match(Variant &&variant, Matchers &&... matchers) -> decltype(auto)
    {
        return std::visit(overload(std::forward<Matchers>(matchers) ...), std::forward<Variant>(variant));
    }
}
#endif //FUN_OVERLOAD_HPP
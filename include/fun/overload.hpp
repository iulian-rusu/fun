#ifndef FUN_OVERLOAD_HPP
#define FUN_OVERLOAD_HPP

#include <variant>
#include <fun/class_wrapper.hpp>

namespace fun
{
    /**
     * A class that models the overload set of a series of callable objects.
     * All callable objects must have a different signature for their operator().
     * The result of calling operator() is determined using operator overloading rules.
     *
     * @tparam  Callables The types of the callable objects that determine this functor's behaviour
     */
    template<typename... Callables>
    struct overload_set : Callables ...
    {
        using Callables::operator() ...;
    };

    template<typename... Callables>
    overload_set(Callables &&...) -> overload_set<class_wrapper_t<Callables> ...>;

    namespace detail
    {
        template<typename... Callables>
        constexpr auto overload_impl(Callables &&... callables) noexcept
        {
            return overload_set<std::remove_reference_t<Callables> ...>{std::forward<Callables>(callables) ...};
        }
    }

    /**
     * Creates an overload set that contains all the given function signatures at once.
     *
     * @tparam  Callables The types of the callable objects that will be combined into one overload set
     * @param   callables The callable objects
     * @return  The overload set instance
     */
    template<traits::callable... Callables>
    [[nodiscard]] constexpr auto overload(Callables &&... callables) noexcept
    {
        return detail::overload_impl(class_wrapper_t<Callables>(std::forward<Callables>(callables)) ...);
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
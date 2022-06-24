#ifndef FUN_OVERLOAD_HPP
#define FUN_OVERLOAD_HPP

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

        /**
         * Extends the current overload set with more callables.
         *
         * @tparam  MoreCallables The types of the callable objects to be added to the overload set
         * @param   callables The actual callable objects to be forwarded
         * @return  A new overload set that contains new and old callables
         */
        template<traits::callable... MoreCallables>
        [[nodiscard]] constexpr auto extend(MoreCallables &&... callables) const noexcept
        {
            return extend_impl(std::forward<class_wrapper_t<MoreCallables>>(class_wrapper_t<MoreCallables>(callables))...);
        }

    private:
        template<typename... MoreCallables>
        constexpr auto extend_impl(MoreCallables &&... callables) const noexcept
        {
            using this_t = decltype(*this);

            return overload_set<std::decay_t<this_t>, std::decay_t<MoreCallables> ...>{
                std::forward<this_t>(*this),
                std::forward<MoreCallables>(callables) ...
            };
        }
    };

    template<typename... Callables>
    overload_set(Callables &&...) -> overload_set<class_wrapper_t<Callables> ...>;

    namespace detail
    {
        template<typename... Callables>
        constexpr auto overload_impl(Callables &&... callables) noexcept
        {
            return overload_set<std::decay_t<Callables> ...>{std::forward<Callables>(callables) ...};
        }
    }

    /**
     * Creates an overload set that contains all the given function signatures at once.
     *
     * @tparam  Callables The types of the callable objects that will be combined into one class
     * @param   callables The callable objects
     * @return  The overload set instance
     */
    template<traits::callable... Callables>
    [[nodiscard]] constexpr auto overload(Callables &&... callables) noexcept
    {
        return detail::overload_impl(class_wrapper_t<Callables>(std::forward<Callables>(callables)) ...);
    }
}
#endif //FUN_OVERLOAD_HPP
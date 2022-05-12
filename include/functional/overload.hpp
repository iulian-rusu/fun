#ifndef FUNCTIONAL_OVERLOAD_HPP
#define FUNCTIONAL_OVERLOAD_HPP

#include <functional/class_wrapper.hpp>

/**
 * A class that behaves like any of the given callable objects at the same time.
 * All callable objects must have a different signature for their operator().
 * The result of calling operator() is determined using operator overloading rules.
 *
 * @tparam  Callables The types of the callable objects that determine this function's behaviour.
 */
template<typename... Callables>
struct overloaded : Callables ...
{
    using Callables::operator() ...;

    /**
     * Extends the current overloaded functor with more callables.
     *
     * @tparam  MoreCallables The types of the callable objects to be added to the overload set.
     * @param   callables The actual callable objects to be forwarded.
     * @return  A new overloaded object that combines new and old callables.
     */
    template<typename... MoreCallables>
    [[nodiscard]] constexpr auto extend(MoreCallables &&... callables) const
    {
        return extend_impl(std::forward<class_wrapper_t<MoreCallables>>(class_wrapper_t<MoreCallables>(callables))...);
    }

private:
    template<typename... MoreCallables>
    constexpr auto extend_impl(MoreCallables &&... callables) const
    {
        using this_t = decltype(*this);

        return overloaded<std::decay_t<this_t>, std::decay_t<MoreCallables> ...>
        {
            std::forward<this_t>(*this),
            std::forward<MoreCallables>(callables) ...
        };
    }
};

template<typename... Callables>
overloaded(Callables &&...) -> overloaded<class_wrapper_t<Callables> ...>;

namespace detail
{
    template<typename... Callables>
    constexpr auto overload_impl(Callables &&... callables)
    {
        return overloaded<std::decay_t<Callables> ...>{std::forward<Callables>(callables) ...};
    }
}

/**
 * Creates an overloaded object that contains all the given function signatures at once.
 *
 * @tparam  Callables The types of the callable objects that will be combined into one class.
 * @param   callables The callable objects.
 * @return  An instance of overloaded.
 */
template<typename... Callables>
[[nodiscard]] constexpr auto overload(Callables &&... callables)
{
    return detail::overload_impl(class_wrapper_t<Callables>(std::forward<Callables>(callables)) ...);
}

#endif //FUNCTIONAL_OVERLOAD_HPP

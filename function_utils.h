#ifndef __FUNCTION_UTILS_H__
#define __FUNCTION_UTILS_H__

#include <utility>

/**
 * A class that behaves like any of the given callable objects at the same time.
 * All callable objects must have a different signature for their operator().
 * The result of calling operator() on a polymorphic_function is determined using operator overloading rules.
 *
 * @tparam Callables The types of the callable objects that determine this function's behaviour.
 */
template<typename ... Callables>
class polymorphic_function : Callables ...
{
public:
    using Callables::operator() ...;

    template<typename ... T>
    polymorphic_function(T &&... callables) : Callables(std::forward<T>(callables)) ...
    {}

    /**
     * Extends the current polymorphic function with more callables.
     *
     * @tparam MoreCallables The types of the callable objects to be added to the overload set.
     * @param callables The actual callable objects to be forwarded.
     * @return A new polymorphic_function that combines new and old callables.
     */
    template<typename ... MoreCallables>
    [[nodiscard]] auto extend(MoreCallables &&... callables) const
    {
        using this_t = decltype(*this);
        return polymorphic_function<std::decay_t<this_t>, std::decay_t<MoreCallables> ...>
                {
                        std::forward<this_t>(*this),
                        std::forward<MoreCallables>(callables) ...
                };
    }
};

/**
 * Creates a polymorphic_function that contains all the given callables at once.
 *
 * @tparam Callables The types of the callable objects that will be combined into one class.
 * @param callables The callable objects.
 * @return An instance of polymorphic_function.
 */
template<typename ... Callables>
[[nodiscard]] auto combine(Callables &&... callables)
{
    return polymorphic_function<std::decay_t<Callables> ...>{std::forward<Callables>(callables) ...};
}


#endif // __FUNCTION_UTILS_H__

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

    template<typename ... MoreCallables>
    auto extend(MoreCallables &&... callables)
    {
        return polymorphic_function<polymorphic_function, std::decay_t<MoreCallables> ...>
                {
                std::forward<polymorphic_function>(*this),
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
auto combine(Callables &&... callables)
{
    return polymorphic_function<std::decay_t<Callables> ...>{std::forward<Callables>(callables) ...};
}


#endif // __FUNCTION_UTILS_H__

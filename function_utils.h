#ifndef __FUNCTION_UTILS_H__
#define __FUNCTION_UTILS_H__


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


/**
 * Custom type traits to check if a type is callable.
 * Adapted from https://stackoverflow.com/questions/15393938/find-out-whether-a-c-object-is-callable.
 */

template<typename T>
using no_ref_t = std::remove_reference_t<T>;

template<typename T>
using no_ptr_ref_t = std::remove_pointer_t<no_ref_t<T>>;

template<typename T>
using is_function = std::is_function<no_ptr_ref_t<T>>;

/**
 * Implementation of the is_callable type trait. This general template will be used when T is a primitive.
 * A primitive is callable when it's a function.
 *
 * @tparam is_class Boolean indicating whether T is a class or primitive type.
 * @tparam T The type checked for being a callable.
 */
template<typename T, bool is_class>
class is_callable_impl : public is_function<T>
{
};

/**
 * Partial specialization of is_callable_impl.
 * Used when T is a class type or a reference to it.
 *
 * @tparam T The type checked for being a callable.
 */
template<typename T>
class is_callable_impl<T, true>
{
    struct _fallback
    {
        void operator()()
        {};
    };

    struct _T_and_fallback : T, _fallback
    {
    };

    template<typename U, U>
    struct _is_instance_of;

    template<typename>
    static std::true_type _check_operator(...)
    {
        return std::true_type{};
    };

    template<typename C>
    static std::false_type _check_operator(_is_instance_of<void (_fallback::*)(), &C::operator()> *)
    {
        return std::false_type{};
    };

public:
    using type = decltype(_check_operator<_T_and_fallback>(nullptr));
};

template<typename T>
using is_callable_t = typename is_callable_impl<no_ref_t<T>, std::is_class_v<no_ref_t<T>>>::type;

template<typename T>
constexpr bool is_callable_v = std::is_same_v<std::true_type, is_callable_t<T>>;


#endif // __FUNCTION_UTILS_H__

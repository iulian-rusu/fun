#ifndef __FUNCTION_UTILS_H__
#define __FUNCTION_UTILS_H__

#include "function.h"

template<typename T>
using no_ref_t = std::remove_reference_t<T>;

template<typename T>
using no_ptr_ref_t = std::remove_pointer_t<no_ref_t<T>>;

/**
 * std::is_function<T> is not std::true_type when T is a reference o pointer to a function
 */
template<typename T>
using is_function = std::is_function<no_ptr_ref_t<T>>;

/**
 * Custom type trait to check if a type is callable.
 * Adapted from https://stackoverflow.com/questions/15393938/find-out-whether-a-c-object-is-callable.
 * This general template will be used when T is a primitive.
 * A primitive is callable when it's a function (function pointer).
 *
 * @tparam is_class Boolean indicating whether T is a class or primitive type.
 * @tparam T The type checked for being a callable.
 */
template<typename T, bool is_class>
class _is_callable_impl : public is_function<T>
{
};

template<typename T>
class _is_callable_impl<T, true>
{
    class _fallback
    {
    public:
        void operator()()
        {};
    };

    class _T_and_fallback : public T, public _fallback
    {
    };

    template<typename U, U>
    class _is_instance_of;

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
using is_callable = _is_callable_impl<std::decay_t<T>, std::is_class_v<std::decay_t<T>>>;

template<typename T>
using is_callable_t = typename is_callable<T>::type;

template<typename T>
constexpr bool is_callable_v = std::is_same_v<std::true_type, is_callable_t<T>>;

/**
 * Defines a wrapper type for non-class callables.
 * Class types are left unchanged.
 *
 * @tparam T The callable type to be wrapped if necessary.
 * @tparam is_class Flag to test if T is a class type.
 * @tparam is_callable Flag to test if T can be assigned to a function object.
 */
template<typename T, bool is_class = std::is_class_v<no_ref_t<T>>, bool is_callable = is_callable_v<T>>
class to_class
{
public:
    using type = T;
};

template<typename T>
class to_class<T, false, true>
{
public:
    using type = decltype(function(std::declval<T>()));
};

template<typename T>
class to_class<T, false, false>
{
    // Non-callable primitives are not supported.
};

template<typename T>
using class_t = typename to_class<T>::type;

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
        return _extend_impl(class_t<MoreCallables>(callables) ...);
    }

private:
    template<typename ... MoreCallables>
    [[nodiscard]] auto _extend_impl(MoreCallables &&... callables) const
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
    return _combine_impl(class_t<Callables>(callables) ...);
}

template<typename ... Callables>
auto _combine_impl(Callables &&... callables)
{
    return polymorphic_function<std::decay_t<Callables> ...>(std::forward<Callables>(callables) ...);
}

#endif // __FUNCTION_UTILS_H__

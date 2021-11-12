#ifndef FUNCTIONAL_FUNCTION_CLASS_WRAPPER_HPP
#define FUNCTIONAL_FUNCTION_CLASS_WRAPPER_HPP

#include<functional/function.hpp>

/**
 * Defines a wrapper type for non-class callables.
 * Class types are left unchanged.
 *
 * @tparam  T The callable type to be wrapped if necessary.
 * @tparam  is_class Flag to test if T is a class type.
 * @tparam  is_callable Flag to test if T can be assigned to a function object.
 */
template<typename T, bool is_class = std::is_class_v<T>, bool is_callable = is_callable_v<T>>
struct class_wrapper
{
    using type = T;
};

template<typename T>
struct class_wrapper<T, false, true>
{
    using type = decltype(function(std::declval<T>()));
};

template<typename T>
struct class_wrapper<T, false, false> {};

template<typename T>
using class_wrapper_t = typename class_wrapper<std::decay_t<T>>::type;


#endif //FUNCTIONAL_FUNCTION_CLASS_WRAPPER_HPP

#ifndef FUNCTIONAL_FUNCTIONAL_TRAITS_HPP
#define FUNCTIONAL_FUNCTIONAL_TRAITS_HPP

#include <type_traits>

/*
 * Type trait for using value semantics for trivially copyable types and const references otherwise.
 */
template<typename T>
using arg_t =
        std::conditional_t
        <
            std::is_trivially_copyable_v<std::remove_cvref_t<T>>,
            std::remove_cvref_t<T>,
            std::remove_cvref_t<T> const &
        >;

/**
 * std::is_function<T> is not std::true_type when T is a reference or pointer to a function
 */
template<typename T>
using is_function = std::is_function<std::remove_pointer_t<std::remove_reference_t<T>>>;

namespace detail
{
    template<typename T, bool IsClass = std::is_class_v<T>>
    struct is_callable_impl : is_function<T> {};

    template<typename T>
    struct is_callable_impl<T, true>
    {
        struct fallback
        {
            void operator()() {};
        };

        struct combined : T, fallback {};

        template<typename U, U>
        class is_instance_of;

        template<typename>
        static std::true_type check(...)
        {
            return {};
        }

        template<typename C>
        static std::false_type check(is_instance_of<void (fallback::*)(), &C::operator()> *)
        {
            return {};
        }

        using type = decltype(check<combined>(nullptr));
        static constexpr bool value = type{};
    };
}

/**
 * Custom type trait to check if a type is callable.
 * Adapted from https://stackoverflow.com/questions/15393938/find-out-whether-a-c-object-is-callable.
 *
 * @tparam  T The type checked for being a callable
 */
template<typename T>
using is_callable = detail::is_callable_impl<std::decay_t<T>>;

template<typename T>
constexpr bool is_callable_v = is_callable<T>::value;

#endif //FUNCTIONAL_FUNCTIONAL_TRAITS_HPP

#ifndef FUN_TRAITS_HPP
#define FUN_TRAITS_HPP

#include <type_traits>
#include <functional>
#include <fun/utility.hpp>

namespace fun::traits
{
    /**
     * std::is_function<T> is not std::true_type when T is a reference or pointer to a function
     */
    template<typename T>
    using is_function = std::is_function<std::remove_pointer_t<std::remove_reference_t<T>>>;

    namespace detail
    {
        template<typename T, bool IsClass = std::is_class_v<T>>
        struct is_callable_impl : is_function<T> {};

        // This is some black magic
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
     * Type trait that detects if an instance of some type can be
     * invoked with any signature of operator().
     *
     * @tparam  T The type checked for being a callable
     */
    template<typename T>
    using is_callable = detail::is_callable_impl<std::decay_t<T>>;

    template<typename T>
    inline constexpr bool is_callable_v = is_callable<T>::value;

    template<typename T>
    concept callable = is_callable_v<T>;

    namespace detail
    {
        template<typename, typename>
        struct is_callable_with_arity_impl;

        template<typename F, std::size_t... Indices>
        struct is_callable_with_arity_impl<F, std::index_sequence<Indices ...>>
        {
            static constexpr bool value = requires (F f) {
                std::invoke(f, lvalue<Indices>() ...);
            };
        };
    }

    /**
     * Type trait that detects if an instance of some type can be
     * invoked with exactly N arguments for operator().
     *
     * @tparam F    The callable type
     * @tparam N    The required number of arguments
     */
    template<typename F, std::size_t N>
    using is_callable_with_arity = detail::is_callable_with_arity_impl<F, std::make_index_sequence<N>>;

    template<typename F, std::size_t N>
    inline constexpr bool is_callable_with_arity_v = is_callable_with_arity<F, N>::value;

    template<typename F, std::size_t N>
    concept callable_with_arity = is_callable_with_arity_v<F, N>;
}
#endif //FUN_TRAITS_HPP
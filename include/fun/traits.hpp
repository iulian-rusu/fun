#ifndef FUN_TRAITS_HPP
#define FUN_TRAITS_HPP

#include <functional>
#include <type_traits>
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
        template<typename F>
        struct is_callable_impl : is_function<F> {};

        // This is some black magic
        template<typename F>
        requires std::is_class_v<F>
        struct is_callable_impl<F>
        {
            struct fallback
            {
                void operator()() {};
            };

            struct derived : F, fallback {};

            template<typename T, T>
            struct instance_of;

            template<typename>
            static std::true_type check(...) { return {}; }

            template<typename T>
            static std::false_type check(instance_of<void (fallback::*)(), &T::operator()> *) { return {}; }

            using type = decltype(check<derived>(nullptr));
            static constexpr bool value = type{};
        };
    }

    /**
     * Type trait that detects if an instance of some type can be
     * invoked with any signature of operator().
     *
     * @tparam F    The type checked for being a callable
     */
    template<typename F>
    using is_callable = detail::is_callable_impl<std::decay_t<F>>;

    template<typename F>
    inline constexpr bool is_callable_v = is_callable<F>::value;

    template<typename F>
    concept callable = is_callable_v<F>;

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
     * Additionally, the call signature must be unambiguous for the given arity,
     * otherwise the result will be false.
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
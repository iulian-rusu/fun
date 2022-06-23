#ifndef FUN_CLASS_WRAPPER_HPP
#define FUN_CLASS_WRAPPER_HPP

#include<fun/function.hpp>

namespace fun
{
    namespace detail
    {
        template<typename T, bool IsClass = std::is_class_v<T>, bool IsCallable = is_callable_v<T>>
        struct class_wrapper_impl
        {
            using type = T;
        };

        template<typename T>
        struct class_wrapper_impl<T, false, true>
        {
            using type = std::remove_reference_t<decltype(function{std::declval<T>()})>;
        };

        template<typename T>
        struct class_wrapper_impl<T, false, false> {};
    }

    /**
     * Defines a wrapper type for non-class callables.
     * Class types are left unchanged.
     *
     * @tparam T The callable type to be wrapped if necessary
     */
    template<typename T>
    using class_wrapper = detail::class_wrapper_impl<std::decay_t<T>>;

    template<typename T>
    using class_wrapper_t = typename class_wrapper<T>::type;
}
#endif //FUN_CLASS_WRAPPER_HPP
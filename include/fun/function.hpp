#ifndef FUN_FUNCTION_HPP
#define FUN_FUNCTION_HPP

// Define special symbol for GCC compilers
#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#define FUN_GCC
#endif

#include <string>
#include <fun/function_traits.hpp>

// Include GCC-specific name de-mangling header
#ifdef FUN_GCC
#include <cxxabi.h>
#endif

namespace fun
{
    /**
     * Function used to default construct an object.
     *
     * @return  A default constructed Return type object
     */
    template<typename Return, typename... Args>
    static constexpr Return default_function(Args ...)
    requires (std::is_void_v<Return> || std::is_default_constructible_v<Return>)
    {
        if constexpr (!std::is_void_v<Return>) // why can't we have regular void?
            return {};
    }

    /**
     * Template for wrapper over a function pointer.
     *
     * @tparam  Return The return type of the function
     * @tparam  Args The optional parameter types taken by the function
     */
    template<typename Return, typename... Args>
    class function
    {
        Return (*fptr)(Args ...);

    public:
        static std::string_view const fptr_name;
        static std::string_view const class_name;

        constexpr function() noexcept: fptr{default_function<Return, Args ...>} {}

        constexpr function(function<Return, Args ...> const &other) noexcept = default;

        constexpr function(Return (*func)(Args ...)) noexcept: fptr{func} {}

        template<typename Lambda>
        requires std::is_convertible_v<std::remove_cvref<Lambda>, decltype(fptr)>
        constexpr function(Lambda &&lambda) noexcept : fptr{std::forward<Lambda>(lambda)} {}

        constexpr function &operator=(Return (*func)(Args ...)) noexcept
        {
            fptr = func;
            return *this;
        }

        constexpr function &operator=(function<Return, Args ...> const &other) noexcept
        {
            if (this == &other)
                return *this;
            this->fptr = other.fptr;
            return *this;
        }

        template<typename Lambda>
        requires std::is_convertible_v<std::remove_cvref<Lambda>, decltype(fptr)>
        constexpr function &operator=(Lambda &&lambda)
        {
            fptr = std::forward<lambda>(lambda);
            return *this;
        }

        /**
         * This overload is [[nodiscard]] and is enabled if Return is not void.
         *
         * @param   args Pack of arguments used to call the function
         * @return  A value of type Return, the result of the function call
         */
        [[nodiscard]] constexpr auto operator()(Args ... args) const
        requires (!std::is_void_v<Return>)
        {
            return fptr(args ...);
        }

        /**
        * This overload's return type is void and can be discarded.
        *
        * @param    args Pack of arguments used to call the function
        * @return   A value of type Return, the result of the function call
        */
        constexpr auto operator()(Args ... args) const
        requires std::is_void_v<Return>
        {
            fptr(args ...);
        }

        /**
         * Returns a lambda that captures a partial state of the original function.
         *
         * @tparam  FrontArgs The types used in the partial state
         * @param   front_args The actual arguments used to make the partial function
         * @return  A lambda capturing the partial state of the function
         */
        template<typename... FrontArgs>
        [[nodiscard]] constexpr auto bind_front(FrontArgs &&... front_args) const noexcept
        {
            return [...front_args = std::forward<FrontArgs>(front_args), &fptr = *this](auto &&... rest_args)
            {
                return fptr(front_args ..., rest_args ...);
            };
        }

        /**
         * Returns a lambda that captures the functional composition of this object and the received object.
         * f.composed_with(g)(x) <=> (f o g)(x) <=> f(g(x))
         *
         * @tparam  ResultArgs The arguments received by the composition result
         * @param   other_func A constant reference to the composition target
         * @return  A lambda that represents the composition
         */
        template<typename... ResultArgs>
        requires (sizeof... (Args) == 1)
        [[nodiscard]] constexpr auto composed_with(function<Args ..., ResultArgs ...> const &other_func) const noexcept
        {
            return [&other_func = other_func, &fptr = *this](ResultArgs ... args)
            {
                return fptr(other_func(args ...));
            };
        }
    };

    template<typename T>
    static auto *get_type_name()
    {
        char const *result_name = typeid(T).name();
#ifdef FUN_GCC
        // De-mangling uses GCC-specific functions.
    int error_code{};
    char *demangled_name = abi::__cxa_demangle(result_name, nullptr, nullptr, &error_code);
    if (!error_code)
        result_name = demangled_name;
#endif
        return result_name;
    }

    template<typename Return, typename... Args>
    std::string_view const function<Return, Args ...>::fptr_name = get_type_name<Return (*)(Args ...)>();

    template<typename Return, typename... Args>
    std::string_view const function<Return, Args ...>::class_name = get_type_name<function>();

    template<typename... Args>
    using action = function<void, Args ...>;

    template<typename T, typename U>
    using comparator = function<bool, arg_t<T>, arg_t<U>>;

    template<typename T>
    using predicate = function<bool, arg_t<T>>;

    template<typename Return, typename T = Return>
    using transform = function<Return, arg_t<T>>;
}
#endif //FUN_FUNCTION_HPP
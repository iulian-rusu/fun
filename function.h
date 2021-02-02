#ifndef _FUNC_TYPES_H_
#define _FUNC_TYPES_H_

/* Define special symbol for GCC compilers. */
#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#define __GCC_COMPILER__
#endif

#include <type_traits>
#include <string>
#include <stdexcept>

/* Include GCC-specific name de-mangling header. */
#ifdef __GCC_COMPILER__
#include <cxxabi.h>
#endif

/**
 * Template for wrapper over a function pointer.
 *
 * @tparam return_t The return type of the function.
 * @tparam Params The parameter types taken by the function. May be missing.
 */
template<typename return_t, typename ... Params>
class function
{
    return_t (*fptr)(Params ...);

public:
    static const std::string_view fptr_type_name;
    static const std::string_view class_type_name;

    constexpr function() noexcept: fptr(defaults::default_function) {}

    constexpr function(function<return_t, Params ...> const &other) noexcept = default;

    constexpr function(return_t (*func)(Params ...)) noexcept: fptr(func) {}

    template<typename F>
    constexpr function(F const &lambda)
    {
        static_assert(std::is_convertible_v<F, decltype(fptr)>, "cannot convert lambda to function pointer");
        fptr = lambda;
    }

    function &operator=(return_t (*func)(Params ...)) noexcept
    {
        fptr = func;
        return *this;
    }

    function &operator=(function<return_t, Params ...> const &other) noexcept
    {
        if (this == &other)
            return *this;
        this->fptr = other.fptr;
        return *this;
    }

    template<typename F>
    function &operator=(F const &lambda)
    {
        static_assert(std::is_convertible_v<F, decltype(fptr)>, "cannot convert lambda to function pointer");
        fptr = lambda;
        return *this;
    }

    /**
     * This overload is [[nodiscard]] and is enabled if return_t is not void.
     *
     * @tparam maybe_void_t Type used to check if the return type is void.
     * @param params Pack of parameters used to call the function.
     * @return A value of type return_t, the result of the function call.
     */
    template<typename maybe_void_t = return_t,
            typename std::enable_if_t<!std::is_void_v<maybe_void_t>> * = nullptr>
    [[nodiscard]] return_t operator()(Params ... params) const
    {
        return fptr(params ...);
    }

    /**
    * This overload's return is void and can be discarded.
    *
    * @tparam maybe_void_t Type used to check if the return type is void.
    * @param params Pack of parameters used to call the function.
    * @return A value of type return_t, the result of the function call.
    */
    template<typename maybe_void_t = return_t,
            typename std::enable_if_t<std::is_void_v<maybe_void_t>> * = nullptr>
    void operator()(Params ... params) const
    {
        fptr(params ...);
    }

    /**
     * Returns a lambda that captures a partial state of the original function.
     *
     * @tparam PartialParams The types used in the partial state.
     * @param partial_params The actual parameters used to make the partial.
     * @return A lambda capturing the partial state of the function.
     */
    template<typename ... PartialParams>
    [[nodiscard]] auto partial(PartialParams ... partial_params) const noexcept
    {
        return [&](auto &&... rest_params)
        {
            return fptr(partial_params ..., rest_params ...);
        };
    }

    /**
     * Returns a lambda that captures the functional composition of this object and the received object.
     * This method requires that Params is of size 1.
     * f.composed_with(g)(x) <=> (f o g)(x) <=> f(g(x))
     *
     * @tparam CompositionParams The parameters received by the composition result.
     * @param other_func A constant reference to the composition target.
     * @return A lambda that represents the composition.
     */
    template<typename ... CompositionParams>
    [[nodiscard]] auto composed_with(function<Params ..., CompositionParams ...> const &other_func) const noexcept
    {
        static_assert(sizeof ...(Params) == 1, "function with multiple parameters cannot be composed");
        return [&](CompositionParams ... params)
        {
            return fptr(other_func(params ...));
        };
    }

    class defaults
    {
    public:
        template<typename maybe_void_t = return_t,
                typename std::enable_if_t<!std::is_void_v<maybe_void_t>> * = nullptr>
        static return_t default_function([[maybe_unused]] Params ... params)
        {
            static_assert(std::is_default_constructible_v<return_t>,
                          "return type of default function must be default constructible");
            return return_t{};
        }

        template<typename maybe_void_t = return_t,
                typename std::enable_if_t<std::is_void_v<maybe_void_t>> * = nullptr>
        static void default_function([[maybe_unused]] Params ... params) noexcept {}
    };
};

template<typename T>
static const char* demangled_type_name()
{
    const char* result_name = typeid(T).name();
#ifdef __GCC_COMPILER__
    // De-mangling uses GCC-specific functions.
    int error_code;
    char *demangled_name = abi::__cxa_demangle(result_name, NULL, NULL, &error_code);
    if(!error_code)
        result_name = demangled_name;
#endif
    return result_name;
}

template<typename return_t, typename ... Params>
const std::string_view function<return_t, Params ...>::fptr_type_name = demangled_type_name<return_t (*)(Params ...)>();

template<typename return_t, typename ... Params>
const std::string_view function<return_t, Params ...>::class_type_name = demangled_type_name<function>();

template<typename ... Params>
using action = function<void, Params ...>;

template<typename T, typename U>
using comparator = function<bool, T, U>;

template<typename T>
using predicate = function<bool, T>;

#endif // _FUNC_TYPES_H_
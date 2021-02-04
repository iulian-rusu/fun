#ifndef __FUNCTION_H__
#define __FUNCTION_H__

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
 * @tparam Return The return type of the function.
 * @tparam Params The parameter types taken by the function. May be missing.
 */
template<typename Return, typename ... Params>
class Function
{
    Return (*fptr)(Params ...);

public:
    static const std::string_view fptr_name;
    static const std::string_view class_name;

    constexpr Function() noexcept: fptr(Function::default_function) {}

    constexpr Function(Function<Return, Params ...> const &other) noexcept = default;

    constexpr Function(Return (*func)(Params ...)) noexcept: fptr(func) {}

    template<typename Lambda>
    constexpr Function(Lambda const &lambda)
    {
        static_assert(std::is_convertible_v<Lambda, decltype(fptr)>, "cannot convert lambda to function pointer");
        fptr = lambda;
    }

    Function &operator=(Return (*func)(Params ...)) noexcept
    {
        fptr = func;
        return *this;
    }

    Function &operator=(Function<Return, Params ...> const &other) noexcept
    {
        if (this == &other)
            return *this;
        this->fptr = other.fptr;
        return *this;
    }

    template<typename Lambda>
    Function &operator=(Lambda const &lambda)
    {
        static_assert(std::is_convertible_v<Lambda, decltype(fptr)>, "cannot convert lambda to function pointer");
        fptr = lambda;
        return *this;
    }

    /**
     * This overload is [[nodiscard]] and is enabled if Return is not void.
     *
     * @tparam R Template parameter used for SFINAE template type deduction.
     * @param params Pack of parameters used to call the Function.
     * @return A value of type Return, the result of the Function call.
     */
     template<typename R = Return>
    [[nodiscard]] auto operator()(Params ... params) const -> std::enable_if_t<!std::is_void_v<R>, R>
    {
        return fptr(params ...);
    }

    /**
    * This overload's return type is void and can be discarded.
    *
    * @tparam R Template parameter used for SFINAE template type deduction.
    * @param params Pack of parameters used to call the Function.
    * @return A value of type Return, the result of the Function call.
    */
    template<typename R = Return>
    auto operator()(Params ... params) const -> std::enable_if_t<std::is_void_v<R>, void>
    {
        fptr(params ...);
    }

    /**
     * Returns a lambda that captures a partial state of the original Function.
     *
     * @tparam PartialParams The types used in the partial state.
     * @param partial_params The actual parameters used to make the partial.
     * @return A lambda capturing the partial state of the Function.
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
    [[nodiscard]] auto composed_with(Function<Params ..., CompositionParams ...> const &other_func) const noexcept
    {
        static_assert(sizeof ...(Params) == 1, "function with multiple parameters cannot be composed");
        return [&](CompositionParams ... params)
        {
            return fptr(other_func(params ...));
        };
    }

    /**
     * Default static member function used to default construct a Function object.
     * This function will be replaced with a different version for void return types using SFINAE.
     *
     * @tparam R Template parameter used for SFINAE template type deduction.
     * @return A default constructed Return type object.
     */
    template<typename R = Return>
    static auto default_function(Params ... ) -> std::enable_if_t<!std::is_void_v<R>, R>
    {
        static_assert(std::is_default_constructible_v<R>,
                      "return type of default function must be default constructible");
        return R{};
    }

    template<typename R = Return>
    static auto default_function(Params ... ) noexcept -> std::enable_if_t<std::is_void_v<R>, void> {}
};

template<typename T>
static const char* get_type_name()
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

template<typename Return, typename ... Params>
const std::string_view Function<Return, Params ...>::fptr_name = get_type_name<Return (*)(Params ...)>();

template<typename Return, typename ... Params>
const std::string_view Function<Return, Params ...>::class_name = get_type_name<Function>();

template<typename ... Params>
using Action = Function<void, Params ...>;

template<typename T, typename U>
using Comparator = Function<bool, std::remove_reference_t<T> const &, std::remove_reference_t<U> const &>;

template<typename T>
using Predicate = Function<bool, std::remove_reference_t<T> const &>;

#endif // __FUNCTION_H__
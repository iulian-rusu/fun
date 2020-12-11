#ifndef _FUNC_TYPES_H_
#define _FUNC_TYPES_H_
/* Define special symbol for GCC compilers. */
#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#define GCC_DEFINED
#endif
/* Include common headers. */
#include <type_traits>
#include <string>
#include <stdexcept>
/* Include GCC-specific ptr_type_name de-mangling header. */
#ifdef GCC_DEFINED
#include <cxxabi.h>
#endif

/**
 * Class that encapsulates a function pointer.
 *
 * @tparam return_t The return type of the function.
 * @tparam param_t The parameter types taken by the function. Can be empty.
 */
template<typename return_t, typename ... param_t>
class func_t
{
    return_t (*func_ptr)(param_t ...);

public:
    static const std::string_view ptr_type_name;
    static const std::string_view class_type_name;

    constexpr func_t() noexcept: func_ptr(defaults::default_func)
    {}

    constexpr func_t(const func_t<return_t, param_t ...> &other) noexcept = default;

    constexpr func_t(return_t (*func)(param_t ...)) noexcept: func_ptr(func)
    {}

    func_t &operator=(const return_t (*&other)(param_t ...)) noexcept
    {
        func_ptr = other;
        return *this;
    }

    func_t &operator=(const func_t<return_t, param_t ...> &other) noexcept
    {
        if (this == &other)
            return *this;
        this->func_ptr = other.func_ptr;
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
    [[nodiscard]] return_t operator()(param_t ... params) const
    {
        return func_ptr(params ...);
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
    void operator()(param_t ... params) const
    {
        func_ptr(params ...);
    }

    /**
     * Returns a lambda that captures a partial state of the original function.
     *
     * @tparam partial_param_t The types used in the partial state.
     * @param params The actual parameters used to make the partial.
     * @return A lambda capturing the partial state of the function.
     */
    template<typename ... partial_param_t>
    [[nodiscard]] auto partial(partial_param_t ... params) const noexcept
    {
        return [&](auto &&... rest)
        {
            return func_ptr(params ..., rest ...);
        };
    }
    /**
     * Returns a lambda that captures the functional composition of this object and the received object.
     * This method requires that param_t is of size 1.
     * f.composed_with(g)(x) <=> (f o g)(x) <=> f(g(x))
     *
     * @tparam other_param_t The parameters received by the other function.
     * @param other_func A constant reference to the composition target.
     * @return A lambda that represents the composition.
     */
    template<typename ... other_param_t>
    [[nodiscard]] auto composed_with(func_t<param_t ..., other_param_t ...> const& other_func) const noexcept
    {
        static_assert(sizeof ...(param_t) == 1, "function with multiple parameters cannot be composed");
        return [&](other_param_t ... params)
        {
            return func_ptr(other_func.func_ptr(params ...));
        };
    }

    class defaults
    {
    public:
        template<typename maybe_void_t = return_t,
                typename std::enable_if_t<!std::is_void_v<maybe_void_t>> * = nullptr>
        static return_t default_func([[maybe_unused]] param_t ... params)
        {
            static_assert(std::is_default_constructible_v<return_t>,
                          "return type of default function must be default constructible");
            return return_t{};
        }

        template<typename maybe_void_t = return_t,
                typename std::enable_if_t<std::is_void_v<maybe_void_t>> * = nullptr>
        static void default_func([[maybe_unused]] param_t ... params) noexcept
        {}
    };
};

template<typename T>
static const char* demangled_type_name()
{
    const char* result_name = typeid(T).name();
#ifdef GCC_DEFINED
    // De-mangling uses GCC-specific functions.
    int error_code;
    char *demangled_name = abi::__cxa_demangle(result_name, NULL, NULL, &error_code);
    if(!error_code)
        result_name = demangled_name;
#endif
    return result_name;
}

template<typename return_t, typename ... param_t>
const std::string_view func_t<return_t, param_t ...>::ptr_type_name = demangled_type_name<return_t (*)(param_t ...)>();

template<typename return_t, typename ... param_t>
const std::string_view func_t<return_t, param_t ...>::class_type_name = demangled_type_name<func_t>();

template<typename ... param_t>
using void_func_t = func_t<void, param_t ...>;

template<typename T, typename U>
using comparator_t = func_t<bool, T, U>;

template<typename T>
using predicate_t = func_t<bool, T>;
#endif // _FUNC_TYPES_H_
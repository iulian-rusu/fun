#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

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
 * @tparam Params The optional parameter types taken by the function.
 */
template<typename Return, typename ... Params>
class function
{
    Return (*fptr)(Params ...);

public:
    static const std::string_view fptr_name;
    static const std::string_view class_name;

    constexpr function() noexcept: fptr(function::default_function)
    {}

    constexpr function(function<Return, Params ...> const &other) noexcept = default;

    constexpr function(Return (*func)(Params ...)) noexcept: fptr(func)
    {}

    template<typename Lambda>
    constexpr function(Lambda const &lambda)
    {
        static_assert(std::is_convertible_v<Lambda, decltype(fptr)>, "cannot convert lambda to function pointer");
        fptr = lambda;
    }

    constexpr function &operator=(Return (*func)(Params ...)) noexcept
    {
        fptr = func;
        return *this;
    }

    constexpr function &operator=(function<Return, Params ...> const &other) noexcept
    {
        if (this == &other)
            return *this;
        this->fptr = other.fptr;
        return *this;
    }

    template<typename Lambda>
    constexpr function &operator=(Lambda const &lambda)
    {
        static_assert(std::is_convertible_v<Lambda, decltype(fptr)>, "cannot convert lambda to function pointer");
        fptr = lambda;
        return *this;
    }

    /**
     * This overload is [[nodiscard]] and is enabled if Return is not void.
     *
     * @tparam R Template parameter used for SFINAE template type deduction.
     * @param params Pack of parameters used to call the function.
     * @return A value of type Return, the result of the function call.
     */
    template<typename R = Return>
    [[nodiscard]] auto operator()(Params ...params) const -> std::enable_if_t<!std::is_void_v<R>, R>
    {
        return fptr(params ...);
    }

    /**
    * This overload's return type is void and can be discarded.
    *
    * @tparam R Template parameter used for SFINAE template type deduction.
    * @param params Pack of parameters used to call the function.
    * @return A value of type Return, the result of the function call.
    */
    template<typename R = Return>
    auto operator()(Params ... params) const -> std::enable_if_t<std::is_void_v<R>, void>
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
    [[nodiscard]] constexpr auto partial(PartialParams ... partial_params) const noexcept
    {
        return [...partial_params = std::move(partial_params), &fptr = *this](auto &&... rest_params)
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
    [[nodiscard]] constexpr auto
    composed_with(function<Params ..., CompositionParams ...> const &other_func) const noexcept
    {
        static_assert(sizeof ...(Params) == 1, "function with multiple parameters cannot be composed");
        return [&other_func = other_func, &fptr = *this](CompositionParams ... params)
        {
            return fptr(other_func(params ...));
        };
    }

    /**
     * Default static member function used to default construct a function object.
     * This function will be replaced with a different version for void return types using SFINAE.
     *
     * @tparam R Template parameter used for SFINAE template type deduction.
     * @return A default constructed Return type object.
     */
    template<typename R = Return>
    static constexpr auto default_function(Params ...) -> std::enable_if_t<!std::is_void_v<R>, R>
    {
        static_assert(std::is_default_constructible_v<R>,
                      "return type of default function must be default constructible");
        return R{};
    }

    template<typename R = Return>
    static constexpr auto default_function(Params ...) noexcept -> std::enable_if_t<std::is_void_v<R>, void>
    {}
};

template<typename T>
static const char *get_type_name()
{
    const char *result_name = typeid(T).name();
#ifdef __GCC_COMPILER__
    // De-mangling uses GCC-specific functions.
    int error_code;
    char *demangled_name = abi::__cxa_demangle(result_name, NULL, NULL, &error_code);
    if (!error_code)
        result_name = demangled_name;
#endif
    return result_name;
}

template<typename Return, typename ... Params>
const std::string_view function<Return, Params ...>::fptr_name = get_type_name<Return (*)(Params ...)>();

template<typename Return, typename ... Params>
const std::string_view function<Return, Params ...>::class_name = get_type_name<function>();

template<typename ... Params>
using action = function<void, Params ...>;

// Helper type definition for more compact code.
// Returns the raw type of T, stripped of any const, volatile or reference modifiers.
template<typename T>
using raw_type = std::remove_cvref_t<T>;

// The type definition below adapt to trivially copyable types and pass them by value.
// Non-trivially copyable types are passed by const reference.

template<typename T, typename U>
using comparator = function<bool,
        std::conditional_t<std::is_trivially_copyable_v<raw_type<T>>, raw_type<T>, raw_type<T> const &>,
        std::conditional_t<std::is_trivially_copyable_v<raw_type<U>>, raw_type<U>, raw_type<U> const &>>;

template<typename T>
using predicate = function<bool,
        std::conditional_t<std::is_trivially_copyable_v<raw_type<T>>, raw_type<T>, raw_type<T> const &>>;

template<typename Return, typename T = Return>
using transform = function<Return,
        std::conditional_t<std::is_trivially_copyable_v<raw_type<T>>, raw_type<T>, raw_type<T> const &>>;

template<typename T>
using no_ptr_ref_t = std::remove_pointer_t<std::remove_reference_t<T>>;

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
template<typename T, bool is_class = std::is_class_v<std::remove_reference_t<T>>, bool is_callable = is_callable_v<T>>
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
    constexpr polymorphic_function(T &&... callables)
    noexcept((std::is_nothrow_move_constructible_v<Callables> && ...))
            : Callables(std::forward<T>(callables)) ...
    {}

    /**
     * Extends the current polymorphic function with more callables.
     *
     * @tparam MoreCallables The types of the callable objects to be added to the overload set.
     * @param callables The actual callable objects to be forwarded.
     * @return A new polymorphic_function that combines new and old callables.
     */
    template<typename ... MoreCallables>
    [[nodiscard]] constexpr auto extend(MoreCallables &&... callables) const
    {
        return _extend_impl(std::forward<class_t<MoreCallables>>(class_t<MoreCallables>(callables))...);
    }

private:
    template<typename ... MoreCallables>
    [[nodiscard]] constexpr auto _extend_impl(MoreCallables &&... callables) const
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
[[nodiscard]] constexpr auto combine(Callables &&... callables)
{
    return _combine_impl(std::forward<class_t<Callables>>(class_t<Callables>(callables)) ...);
}

template<typename ... Callables>
constexpr auto _combine_impl(Callables &&... callables)
{
    return polymorphic_function<std::decay_t<Callables> ...>(std::forward<Callables>(callables) ...);
}


#endif // __FUNCTIONAL_H__

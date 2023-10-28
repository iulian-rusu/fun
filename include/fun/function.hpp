#ifndef FUN_FUNCTION_HPP
#define FUN_FUNCTION_HPP

// Define special symbol for GCC compilers
#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
#define FUN_GCC
#endif

#include <string_view>

// Include GCC-specific name de-mangling header
#ifdef FUN_GCC
#include <cxxabi.h>
#endif

#include <fun/utility.hpp>

namespace fun
{
    namespace detail
    {
        template<typename T>
        static auto *type_name()
        {
            char const *name = typeid(T).name();
#ifdef FUN_GCC
            // De-mangling uses GCC-specific functions.
        int error_code{};
        char *demangled_name = abi::__cxa_demangle(name, nullptr, nullptr, &error_code);
        if (!error_code)
            name = demangled_name;
#endif
            return name;
        }
    }

    template<typename Signature>
    class function;

    /**
     * Wrapper over a function pointer.
     *
     * @tparam Return   The return type of the function
     * @tparam Args     The parameters of the function
     */
    template<typename Return, typename... Args>
    class function<Return(Args ...)>
    {
        Return (*_fptr)(Args ...) = nullptr;

    public:
        [[nodiscard]] static std::string_view fptr_name() noexcept
        {
            return detail::type_name<Return (*)(Args ...)>();
        };

        [[nodiscard]] static std::string_view class_name() noexcept
        {
            return detail::type_name<function<Return(Args ...)>>();
        }

        constexpr function(function<Return(Args ...)> const &other) noexcept = default;

        constexpr function(Return (*f)(Args ...)) noexcept : _fptr{f} {}

        template<std::invocable<Args ...> F>
        requires std::is_convertible_v<std::decay_t<F>,  Return (*)(Args ...)>
        constexpr function(F const &f) noexcept : _fptr{f} {}

        constexpr function &operator=(Return (*f)(Args ...)) noexcept
        {
            _fptr = f;
            return *this;
        }

        constexpr function &operator=(function<Return(Args ...)> const &other) noexcept
        {
            if (this == &other)
                return *this;
            _fptr = other._fptr;
            return *this;
        }

        template<std::convertible_to<Return (*)(Args ...)> F>
        constexpr function &operator=(F const &f) noexcept
        {
            _fptr = f;
            return *this;
        }

        [[nodiscard]] constexpr auto operator()(Args... args) const noexcept(noexcept(_fptr(std::move(args) ...)))
        {
            return _fptr(std::move(args) ...);
        }
    };

    template<typename Return, typename... Args>
    function(Return (*)(Args ...)) -> function<Return(Args ...)>;

    template<typename... Args>
    using action = function<void(Args ...)>;

    template<typename T, typename U>
    using comparator = function<bool(T, U)>;

    template<typename T>
    using predicate = function<bool(T)>;

    template<typename Return, typename T = Return>
    using transform = function<Return(T)>;
}
#endif //FUN_FUNCTION_HPP
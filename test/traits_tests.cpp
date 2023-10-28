#include <fun.hpp>

namespace fun::tests
{
    using not_callabe = std::string_view;

    template<typename... Args>
    using callable_with_args = decltype([](Args ...){});

    struct overloaded_callable
    {
        auto operator()() {}
        auto operator()(int) {}
        auto operator()(std::string_view) {}
        auto operator()(std::string_view, std::vector<int> &&) {}
    };

    static_assert(traits::is_callable_v<callable_with_args<>>);
    static_assert(traits::is_callable_v<callable_with_args<> &>);
    static_assert(traits::is_callable_v<callable_with_args<> &&>);
    static_assert(traits::is_callable_v<callable_with_args<> const &>);
    static_assert(traits::is_callable_v<callable_with_args<> const &&>);
    static_assert(traits::is_callable_v<callable_with_args<int>>);
    static_assert(traits::is_callable_v<callable_with_args<int> &>);
    static_assert(traits::is_callable_v<callable_with_args<int> &&>);
    static_assert(traits::is_callable_v<callable_with_args<int> const &>);
    static_assert(traits::is_callable_v<callable_with_args<int> const &&>);
    static_assert(traits::is_callable_v<callable_with_args<int, std::string_view>>);
    static_assert(traits::is_callable_v<callable_with_args<double *, std::vector<int> &&>>);
    static_assert(traits::is_callable_v<overloaded_callable>);
    static_assert(traits::is_callable_v<void()>);
    static_assert(traits::is_callable_v<void(...)>);
    static_assert(traits::is_callable_v<int(*)(int, double)>);

    static_assert(traits::is_callable_v<not_callabe> == false);
    static_assert(traits::is_callable_v<void> == false);

    static_assert(traits::callable_with_arity<void(), 0>);
    static_assert(traits::callable_with_arity<void(...), 0>);
    static_assert(traits::callable_with_arity<void(...), 1>);
    static_assert(traits::callable_with_arity<void(...), 2>);
    static_assert(traits::callable_with_arity<void(...), 3>);
    static_assert(traits::callable_with_arity<callable_with_args<>, 0>);
    static_assert(traits::callable_with_arity<callable_with_args<int>, 1>);
    static_assert(traits::callable_with_arity<callable_with_args<int, double>, 2>);
    static_assert(traits::callable_with_arity<callable_with_args<int, double, char>, 3>);
    static_assert(traits::callable_with_arity<overloaded_callable, 0>);
    static_assert(traits::callable_with_arity<overloaded_callable, 2>);

    static_assert(traits::callable_with_arity<void, 0> == false);
    static_assert(traits::callable_with_arity<overloaded_callable, 3> == false);
    // This will fail because calling overloaded_callable with one argument is ambiguous
    static_assert(traits::callable_with_arity<overloaded_callable, 1> == false);
}